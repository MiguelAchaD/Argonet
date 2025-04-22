BENCHMARK_ASCII="
▗▄▄▖ ▗▄▄▄▖▗▖  ▗▖ ▗▄▄▖▗▖ ▗▖▗▖  ▗▖ ▗▄▖ ▗▄▄▖ ▗▖ ▗▖
▐▌ ▐▌▐▌   ▐▛▚▖▐▌▐▌   ▐▌ ▐▌▐▛▚▞▜▌▐▌ ▐▌▐▌ ▐▌▐▌▗▞▘
▐▛▀▚▖▐▛▀▀▘▐▌ ▝▜▌▐▌   ▐▛▀▜▌▐▌  ▐▌▐▛▀▜▌▐▛▀▚▖▐▛▚▖ 
▐▙▄▞▘▐▙▄▄▖▐▌  ▐▌▝▚▄▄▖▐▌ ▐▌▐▌  ▐▌▐▌ ▐▌▐▌ ▐▌▐▌ ▐▌
"

update_progress_bar() {
    local current=$1
    local total=$2
    local lost=$3
    local hit=$4
    local miss=$5
    local width=50
    
    local percent=$((current * 100 / total))
    local filled=$((width * current / total))
    
    printf "\r\033[K["
    printf "%*s" "$filled" | tr ' ' '='
    printf "%*s" $((width - filled)) | tr ' ' ' '
    
    printf "] %3d%% (%d/%d) | Lost: %d | Hit: %d | Miss: %d" \
        "$percent" "$current" "$total" "$lost" "$hit" "$miss"
}

###################################################
## GLOBAL VARIABLES ###############################
###################################################
INPUT_FILE="http_logs.txt"
OUTPUT_FILE="response_codes.log"
VERBOSE=false
MAX_PARALLEL_JOBS=10
PING_TIMEOUT=3

###################################################
## PROGRAM ARGUMENTS ##############################
###################################################
print_help() {
    echo "Usage: $0 [OPTIONS]"
    echo "Options:"
    echo "  --input FILE      Specify input file with URLs (default: http_logs.txt)"
    echo "  --output FILE     Specify output log file (default: response_codes.log)"
    echo "  --parallel N      Number of parallel jobs (default: 10)"
    echo "  --ping-timeout N  Ping timeout in seconds (default: 3)"
    echo "  --verbose         Enable verbose output"
    echo "  --help            Display this help message"
    exit 0
}

while [[ $# -gt 0 ]]; do    
    case "$1" in
        --input)
            INPUT_FILE="$2"
            shift 2
            ;;
        --output)
            OUTPUT_FILE="$2"
            shift 2
            ;;
        --parallel)
            MAX_PARALLEL_JOBS="$2"
            shift 2
            ;;
        --ping-timeout)
            PING_TIMEOUT="$2"
            shift 2
            ;;
        --verbose)
            VERBOSE=true
            shift
            ;;
        --help)
            print_help
            ;;
        *)
            echo "Unknown option: $1"
            print_help
            ;;
    esac
done

> "$OUTPUT_FILE"

###################################################
## BENCHMARK FUNCTIONS ############################
###################################################
check_host() {
    local host="$1"
    
    if timeout "$PING_TIMEOUT" ping -c 1 "$host" > /dev/null 2>&1; then
        echo "online"
    else
        echo "offline"
    fi
}


call_url() {
    local host="$1"
    local endpoint="$2"
    local method="$3"
    local url="http://$host$endpoint"
    
    local host_status=$(check_host "$host")
    
    local stats_file="/tmp/benchmark_stats.txt"
    
    if [ "$host_status" == "offline" ]; then
        flock -x "$stats_file" bash -c "echo 'lost' >> \"$stats_file\""
        
        echo "$(date '+%Y-%m-%d %H:%M:%S') | Host: $host | Status: $host_status | Skipped request" >> "$OUTPUT_FILE"
    else
        local http_code=$(curl -s -D- -o /dev/null -w "%{http_code}" -X "$method" "$url" -H "Content-Type: */*")
        
        if [[ "$http_code" =~ ^2[0-9]{2}$ ]]; then
            flock -x "$stats_file" bash -c "echo 'hit' >> \"$stats_file\""
        else
            flock -x "$stats_file" bash -c "echo 'miss' >> \"$stats_file\""
        fi
        
        echo "$(date '+%Y-%m-%d %H:%M:%S') | Host: $host | Status: $host_status | Method: $method | URL: $url | HTTP Code: $http_code" >> "$OUTPUT_FILE"
    fi
}

if [ ! -f "$INPUT_FILE" ]; then
    echo "Error: Input file '$INPUT_FILE' not found!"
    exit 1
fi

###################################################
## PARALLELISM FUNCTIONS ##########################
###################################################
declare -a pids
if [[ $VERBOSE -eq true ]]; then
    echo "$BENCHMARK_ASCII" 
fi

stats_file="/tmp/benchmark_stats.txt"
> "$stats_file"

max_entries=$(wc -l < $INPUT_FILE)
entry_index=0
job_count=0
lost=0
hit=0
miss=0

if [[ $VERBOSE -eq true ]]; then
    update_progress_bar 0 "$max_entries" $lost $hit $miss
fi

# START TIME!
start=$(date +%s.%N)

while IFS=';' read -r host endpoint method; do
    ((entry_index++))
    call_url "$host" "$endpoint" "$method" &
    
    pids+=($!)
    
    if [[ -f "$stats_file" ]]; then
        lost=$(grep -c "lost" "$stats_file")
        hit=$(grep -c "hit" "$stats_file")
        miss=$(grep -c "miss" "$stats_file")
    fi
    
    if [[ $VERBOSE -eq true ]]; then
        update_progress_bar "$entry_index" "$max_entries" $lost $hit $miss
    fi
    
    ((job_count++))
    if [ $job_count -ge $MAX_PARALLEL_JOBS ]; then
        wait -n
        ((job_count--))
    fi
done < "$INPUT_FILE"

wait

# END TIME!
end=$(date +%s.%N)

if [[ -f "$stats_file" ]]; then
    lost=$(grep -c "lost" "$stats_file")
    hit=$(grep -c "hit" "$stats_file")
    miss=$(grep -c "miss" "$stats_file")
fi

update_progress_bar "$max_entries" "$max_entries" $lost $hit $miss
printf "\n"

rm -f "$stats_file"

runtime=$(echo "$end - $start" | bc)
echo "RUNTIME ::: $runtime"
