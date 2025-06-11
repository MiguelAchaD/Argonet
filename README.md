<p align="center">
  <img src="assets/static/Logo.svg" alt="Argonet Logo" width="200"/>
</p>

<h1 align="center">Argonet</h1>
<p align="center">
  <em>Smart proxy server with HTTP request analysis and malicious website detection.</em>
</p>

<p align="center">
  <a href="https://github.com/MiguelAchaD/Argonet/releases/latest">
    <img alt="Release" src="https://img.shields.io/github/v/release/MiguelAchaD/Argonet?style=flat-square">
  </a>
  <a href="LICENSE">
    <img alt="License" src="https://img.shields.io/github/license/MiguelAchaD/Argonet?style=flat-square">
  </a>
</p>

---

## 🚀 Features

- HTTP proxy with analysis engine
- Malicious site detection
- Easy build and installation
- Linux support

---

## 💿 Installation

### 🔧 Build from Source

#### 1. Clone the repository

```bash
git clone https://github.com/MiguelAchaD/Argonet.git
cd Argonet
```

#### 2. Install dependencies

**Manual (APT-based distros):**
```bash
sudo apt-get install make g++ openssl libcurl4-openssl-dev nlohmann-json3-dev libsqlite3-dev
```

**Automatic (supported package managers only):**
```bash
./installation/install.sh
```

#### 3. Build

```bash
make
```
#### 3. Run

```bash
./bin/argonet -v
```
#### 3. Configure

Go to a browser, and type the IP address of the server and its port (e.g.: 192.168.1.172:8081). Login with the default credentials (admin, 1234), and configure the server!

---

## 🖥️ Platform Support

| OS      | Supported | Notes                   |
|---------|-----------|-------------------------|
| Linux   | ✅         | Tested on Ubuntu/Debian |
| macOS   | ❌         | Not currently supported |
| Windows | ❌         | Use WSL as workaround   |

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
