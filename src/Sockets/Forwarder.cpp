#include "Forwarder.hpp"
#include "Logger.hpp"

#include <cstring>
#include <unistd.h>
#include <vector>
#include <zlib.h>
#include <sstream>

proxyServer::Forwarder::Forwarder(unsigned short int t_port_number) : Socket(t_port_number) {}

proxyServer::Forwarder::~Forwarder() {
    disconnect();
}

proxyServer::petitionPacket proxyServer::Forwarder::execute(proxyServer::petitionPacket t_packet) {
    if (!t_packet.isResolved) {
        t_packet.isForwarder = false;
        return t_packet;
    }
  
    if (connect(t_packet.host, 80)) {
        sendRequest(t_packet);
        std::string response = receiveResponse();
        
        // Parse headers and body
        size_t header_end = response.find("\r\n\r\n");
        if (header_end != std::string::npos) {
            std::string headers = response.substr(0, header_end + 4);
            std::string body = response.substr(header_end + 4);
            
            // Check if response is gzip encoded
            if (headers.find("Content-Encoding: gzip") != std::string::npos) {
                // Decompress the body
                std::string decompressed_body = decompressGzip(body);
                if (!decompressed_body.empty()) {
                    // Remove Content-Encoding and Content-Length headers
                    headers = removeHeader(headers, "Content-Encoding");
                    headers = removeHeader(headers, "Content-Length");
                    
                    // Add new Content-Length header
                    std::string new_header = "Content-Length: " + std::to_string(decompressed_body.length()) + "\r\n";
                    // Insert before the final \r\n\r\n
                    headers.insert(headers.length() - 4, new_header);
                    
                    // Combine headers and decompressed body
                    response = headers + decompressed_body;
                } else {
                    Logger::log("Failed to decompress response body", Logger::LogType::ERROR);
                    return t_packet;
                }
            }
        }
        
        t_packet.response = FastString(response);
        t_packet.isForwarder = true;
    } else {
        t_packet.isResolved = false;
        t_packet.isForwarder = false;
    }
    return t_packet;
}

std::string proxyServer::Forwarder::sendRequest(const proxyServer::petitionPacket& packet) {
    std::string request = packet.toString();
    ssize_t bytes_sent = send(socket_fd, request.c_str(), request.length(), 0);
    
    if (bytes_sent == -1) {
        Logger::log("Send failed: " + std::string(strerror(errno)), Logger::LogType::ERROR);
        return "";
    }
    return std::to_string(bytes_sent);
}

bool proxyServer::Forwarder::connect(const FastString& host, int port) {
    struct addrinfo hints, *servinfo;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    int rv = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &servinfo);
    if (rv != 0) {
        Logger::log("getaddrinfo error: " + std::string(gai_strerror(rv)), Logger::LogType::ERROR);
        return false;
    }

    if (!createSocket()) {
        freeaddrinfo(servinfo);
        return false;
    }

    setSocketTimeout(5);

    if (::connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        Logger::log("Connection failed: " + std::string(strerror(errno)), Logger::LogType::ERROR);
        freeaddrinfo(servinfo);
        return false;
    }

    freeaddrinfo(servinfo);
    return true;
}

std::string proxyServer::Forwarder::receiveResponse(size_t buffer_size) {
    std::vector<char> buffer(buffer_size);
    std::string full_response;
    bool headers_complete = false;
    size_t content_length = 0;
    size_t body_start = 0;

    while (true) {
        ssize_t bytes_received = recv(socket_fd, buffer.data(), buffer_size - 1, 0);
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';
        full_response.append(buffer.data(), bytes_received);

        if (!headers_complete) {
            size_t header_end = full_response.find("\r\n\r\n");
            if (header_end != std::string::npos) {
                headers_complete = true;
                body_start = header_end + 4;

                // Extract Content-Length
                size_t cl_pos = full_response.find("Content-Length: ");
                if (cl_pos != std::string::npos) {
                    size_t cl_end = full_response.find("\r\n", cl_pos);
                    if (cl_end != std::string::npos) {
                        std::string cl_str = full_response.substr(cl_pos + 16, cl_end - (cl_pos + 16));
                        try {
                            content_length = std::stoul(cl_str);
                        } catch (const std::exception& e) {
                            Logger::log("Invalid Content-Length header: " + cl_str, Logger::LogType::WARNING);
                            content_length = 0;
                        }
                    }
                }

                // If no Content-Length or Content-Length is 0, we're done
                if (content_length == 0) break;
            }
        } else if (content_length > 0) {
            // Check if we've received the full body
            if (full_response.length() - body_start >= content_length) {
                break;
            }
        }
    }

    return full_response;
}

std::string proxyServer::Forwarder::decompressGzip(const std::string& compressed_data) {
    if (compressed_data.empty()) {
        Logger::log("Empty compressed data", Logger::LogType::WARNING);
        return "";
    }

    z_stream zs;
    memset(&zs, 0, sizeof(zs));

    if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) {
        Logger::log("Failed to initialize zlib", Logger::LogType::ERROR);
        return "";
    }

    zs.next_in = (Bytef*)compressed_data.data();
    zs.avail_in = compressed_data.size();

    int ret;
    char outbuffer[32768];
    std::string outstring;

    do {
        zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
        zs.avail_out = sizeof(outbuffer);

        ret = inflate(&zs, 0);

        if (outstring.size() < zs.total_out) {
            outstring.append(outbuffer, zs.total_out - outstring.size());
        }
    } while (ret == Z_OK);

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        Logger::log("Failed to decompress gzip data: " + std::string(zs.msg ? zs.msg : "Unknown error"), Logger::LogType::ERROR);
        return "";
    }

    return outstring;
}

std::string proxyServer::Forwarder::removeHeader(const std::string& headers, const std::string& header_name) {
    std::string result = headers;
    size_t pos = 0;
    std::string search = header_name + ":";
    
    while ((pos = result.find(search, pos)) != std::string::npos) {
        size_t line_start = result.rfind("\r\n", pos);
        if (line_start == std::string::npos) {
            line_start = 0;
        } else {
            line_start += 2;
        }
        
        size_t line_end = result.find("\r\n", pos);
        if (line_end != std::string::npos) {
            result.erase(line_start, line_end - line_start + 2);
            pos = line_start;
        } else {
            break;
        }
    }
    
    return result;
}

void proxyServer::Forwarder::disconnect() {
    closeSocket();
}
