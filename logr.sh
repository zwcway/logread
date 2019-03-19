#!/bin/bash
#set -x

COL_TIME='t'
COL_LEVEL='l'
COL_FILE='f'
COL_EXTRA='e'

REG_NAME='0-9a-zA-Z_'

# 开启转义符解析
OPT_NORMALIZE_SOLIDUS=0
# 开启详细输出
OPT_BRIEF=1

function print_help() {
	echo "用法: logr [参数]... [文件]..."
	echo "格式化日志"
	echo ""
	echo "  -h, --help           显示帮助内容"
	echo
	echo "  -c, --column         过滤列。多个列使用英文逗号分隔。内置列："
	echo "	                     time(日志生成时间): $COL_TIME"
	echo "	                     level(日志等级)   : $COL_LEVEL"
	echo "	                     file(来源文件)    : $COL_FILE"
	echo "	                     extra(其他)       : $COL_EXTRA"
	echo
	echo "  -f, --filter         过滤日志。多个条件使用逗号分隔。格式如下："
	echo "                         key=val   任意位置查找"
	echo "                         key~val   正则查找"
	echo "                         key>val   数值大于val"
	echo "                         key<val   数值小于val"
	echo "                         key>=val  数值大于等于val"
	echo "                         key<=val  数值小于等于val"
	echo
	echo
	echo "示例："
	echo "tail -f ral-worker.log | logr -c t,uri -f 'cost>1000,uri~^bizas'"
	echo "tail -f ral-worker.log | logr -c t -c uri -f 'cost>1000' -f 'uri~^bizas'"
	echo "         表示仅显示日志时间和接口地址两列，并且只显示耗时大于1000"
	echo "         并且接口路径以bizas开始的日志"
}

function terminal() {
	local no=$2
	echo "$1" >&2
	print_help
	exit ${no:=1}
}
function error() {
  [ $OPT_BRIEF -eq 1 ] && echo "$*" >&2
}

function ewhite() {
	echo -ne "\e[97m"
	echo -n "$1"
	echo -ne "\e[0m"
}

function ered() {
	echo -ne "\e[91m"
	echo -n "$1"
	echo -ne "\e[0m"
}

function eyellow() {
	echo -ne "\e[93m"
	echo -n "$1"
	echo -ne "\e[0m"
}

function eblue() {
	echo -ne "\e[96m"
	echo -n "$1"
	echo -ne "\e[0m"
}

function egrey() {
	echo -ne "\e[90m"
	echo -n "$1"
	echo -ne "\e[0m"
}

function elevel() {
	case $1 in
		WARNING) eyellow "$1";;
		*) ewhite "$1";;
	esac
}

function format() {
	LINE_LOG=()
	local line="$*"
	local regral="^((NOTICE|WARNING|TRACE): ([0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}):  ral-worker \* [0-9]+ \[(rpc.cpp:[0-9]+)\])(.*)"
	if [[ "$line" =~ $regral ]]; then
		format_ral "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}" "${BASH_REMATCH[3]}" "${BASH_REMATCH[5]}" "${BASH_REMATCH[4]}"
		return 0
	fi
	local regapp="^((WARNING|NOTICE|TRACE|DEBUG): ([0-9]{2}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}) \[(.+?:[0-9]+)\]) (.*)"
	if [[ "$line" =~ $regapp ]]; then
		format_app "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}" "${BASH_REMATCH[3]}" "${BASH_REMATCH[5]}" "${BASH_REMATCH[4]}"
		return 0
	fi
	echo "$line"
}

function format_ral() {
	local log_hed="$1"
	local log_level="$2"
	local log_time="$3"
	local log_con="$4"
	local log_file="$5"
	local regral="^\[(.+)\]$"
	local key='' val='' print=false
	local line='' fcol=${#COLUMN[@]}
	
	if [[ "$log_con" =~ $regral ]]; then
		log_con="${BASH_REMATCH[1]}"
		if [[ $fcol -eq 0 ]];then
			for json in $log_con; do
				key="${json%%=*}"
				val="${json#*=}"
				#filter_one "$key" "${val:= }" && print=true
				line="$line $(eblue $key)=$(egrey ${val:= })"
			done
			line="$(eblue $COL_FILE)=$(egrey $log_file) $line"
			line="$(eblue $COL_TIME)=$(egrey $log_time) $line"
			line="$(eblue $COL_LEVEL)=$(elevel $log_level) $line"
		else 
			for json in $log_con; do
				key="${json%%=*}"
				val="${json#*=}"
				filter_one "${key}" "${val:= }" && print=true
				[[ "${COLUMN[$key]}" ]] && line="$line $(eblue $key)=$(egrey ${val:= })"
			done
			[[ "${COLUMN[$COL_FILE]}" ]] && line="$(eblue $COL_FILE)=$(egrey $log_file) $line"
			[[ "${COLUMN[$COL_TIME]}" ]] && line="$(eblue $COL_TIME)=$(egrey $log_time) $line"
			[[ "${COLUMN[$COL_LEVEL]}" ]] && line="$(eblue $COL_LEVEL)=$(elevel $log_level) $line"
		fi
	fi

	$print && echo $line
}

function format_app() {
	local log_hed="$1"
	local log_level="$2"
	local log_time="$3"
	local log_con="$4"
	local log_file="$5"
# TODO 配对匹配 json 中的 中括号，防止json中包含空格
	local regjson="([$REG_NAME]+)\[([^ ]+?)\] (.*)\$"
	local key='' val='' print=false
	local line='' fcol=${#COLUMN[@]}
	
	if [[ $fcol -eq 0 ]];then
		while [[ "$log_con" =~ $regjson ]]; do
			log_con="${BASH_REMATCH[3]}"
			key="${BASH_REMATCH[1]}"
			val="${BASH_REMATCH[2]}"
			#ilter_one "$key" "${val:= }" && print=true
			line="$line $(eblue $key)=$(egrey ${val:= })"
			#LINE_LOG["${BASH_REMATCH[1]}"]="${BASH_REMATCH[2]}"
		done
		line="$(eblue $COL_FILE)=$(egrey $log_file) $line"
		line="$(eblue $COL_TIME)=$(egrey $log_time) $line"
		line="$(eblue $COL_LEVEL)=$(elevel $log_level) $line"
	else 
		while [[ "$log_con" =~ $regjson ]]; do
			log_con="${BASH_REMATCH[3]}"
			key="${BASH_REMATCH[1]}"
			val="${BASH_REMATCH[2]}"
			#ilter_one "$key" "${val:= }" && print=true
			[[ "${COLUMN[$key]}" ]] && line="$line $(eblue $key)=$(egrey ${val:= })"
		done
		[[ "${COLUMN[$COL_FILE]}" ]] && line="$(eblue $COL_FILE)=$(egrey $log_file) $line"
		[[ "${COLUMN[$COL_TIME]}" ]] && line="$(eblue $COL_TIME)=$(egrey $log_time) $line"
		[[ "${COLUMN[$COL_LEVEL]}" ]] && line="$(eblue $COL_LEVEL)=$(elevel $log_level) $line"
	fi
	
	filter_one "$COL_TIME" "${log_time}" && print=true
	filter_one "$COL_LEVEL" "${log_level}" && print=true
	filter_one "$COL_FILE" "${log_file}" && print=true
	
	$print && echo $line
}

function format_json() {
	local token=${1:0:1}
	local key=$2
	( [[ $token = '{' ]] || [[ $token = '[' ]] ) || return 1

	json_tokenize "$1" $key | json_parse
}

function filter_one() {
	local key="$1" var="$2" name opr expr json=''
	
	opr="${FILTER[$key]%% *}"
	expr="${FILTER[$key]#* }"
	
	[[ "$opr" ]] || json=$(format_json "$val" "$key")
	
	if ! [[ "$json" = '' ]]; then
		echo "$json" | while read -r obj; do
			[[ "${FILTER[${obj%%\t*}]}" ]] && val=${obj#*\t}
		done
	fi
	
	case $opr in
		=)
			[[ "${val}" =~ "${expr:= }" ]] || return 1
			;;
		'>='|'>'|'<='|'<')
			[[ "$val" =~ ^[0-9\.]+$ ]] || return 1
			compare_float "$val" "$opr" "$expr" || return 1
			;;
		'~')
			[[ "$val" =~ $expr ]] || return 1
			;;
		*)
			;;
	esac
	:
}

function filter() {
	local opr expr val
	for key in ${!FILTER[*]}; do
		if [[ ${LINE_LOG[$key]} ]]; then
			val="${LINE_LOG[$key]}"
			opr=${FILTER[$key]%% *}
			expr="${FILTER[$key]#* }"
			case $opr in
				=)
					[[ "${val}" =~ "${expr:= }" ]] || return 1
					;;
				'>='|'>'|'<='|'<')
					[[ "$val" =~ ^[0-9\.]+$ ]] || return 1
					compare_float "$val" "$opr" "$expr" || return 1
					;;
				'~')
					[[ "$val" =~ $expr ]] || return 1
					;;
				*)
					;;
			esac
		else
			return 1
		fi
	done
	
	return 0
}

function arg_filter() {
	local regarg="^([$REG_NAME\.]+?)(>=|<=|=|~|>|<)(.*)\$"
	local tmp key opr expr
	local arg="$1"
	
	while :; do
		tmp="${arg%%,*}"
		arg="${arg#*,}"
		if [[ "$tmp" =~ $regarg ]]; then
			key="${BASH_REMATCH[1]}"
			opr="${BASH_REMATCH[2]}"
			expr="${BASH_REMATCH[3]}"
#			[[ "${opr}" = '~' ]] && [[ "${expr}" =~ ' ' ]] && terminal "过滤器格式错误，正则格式错误 '$tmp'" 2
			FILTER["${key}"]="${opr} ${expr}"
		else 
			terminal "过滤器格式错误，未识别的操作符 '$tmp'" 3
		fi
		[[ "x$tmp" = "x$arg" ]] && break
	done
}

function arg_column() {
	local regarg="^[$REG_NAME]+\$"
	local tmp
	local arg="$1"
	
	while :; do
		tmp="${arg%%,*}"
		arg="${arg#*,}"
		if [[ "$tmp" =~ $regarg ]]; then
			COLUMN[${#COLUMN[@]}]="$tmp"
		fi
		[[ "x$tmp" = "x$arg" ]] && break
	done
}

function arg_check() {
	if [[ "x$2" = "x" ]] || [[ "$2" =~ ^- ]]; then
		terminal "$1 必须指定参数" 1
	fi
}

function compare_float() {
    local oldIFS="$IFS" op=$2 x y digitx digity
    IFS='.' x=( ${1##+([0]|[-]|[+])}) y=( ${3##+([0]|[-]|[+])}) IFS="$oldIFS"
    while [[ "${x[1]}${y[1]}" =~ [^0] ]]; do
        digitx=${x[1]:0:1} digity=${y[1]:0:1}
        (( x[0] = x[0] * 10 + ${digitx:-0} , y[0] = y[0] * 10 + ${digity:-0} ))
        x[1]=${x[1]:1} y[1]=${y[1]:1} 
    done
    [[ ${1:0:1} == '-' ]] && (( x[0] *= -1 ))
    [[ ${3:0:1} == '-' ]] && (( y[0] *= -1 ))
    (( ${x:-0} $op ${y:-0} ))
}

function json_tokenize () {
	local GREP
	local ESCAPE='(\\[^u[:cntrl:]]|\\u[0-9a-fA-F]{4})'
	local CHAR='[^[:cntrl:]"\\]'
	local STRING="\"$CHAR*($ESCAPE$CHAR*)*\""
	local CHAR2="[^[:cntrl:]'\]"
	local STRING2="'$CHAR2*($ESCAPE$CHAR2*)*'"
	local NUMBER='-?(0|[1-9][0-9]*)([.][0-9]*)?([eE][+-]?[0-9]*)?'
	local KEYWORD='null|false|true'
	local SPACE='[[:space:]]+'

	local json="$1"
	local pattern="$STRING|$STRING2|$NUMBER|$KEYWORD|$SPACE|."
	local token
	
	while [[ "${json}" =~ $pattern ]]; do
		token="${BASH_REMATCH[0]}"
		echo "$token"
		json="${json#*$token}"
	done
}

function json_parse_array () {
  local index=0
  local ary=''
  read -r token
  case "$token" in
    ']') ;;
    *)
      while :
      do
        json_parse_value "$1" "$index" || return $?
        index=$((index+1))
        ary="$ary""$value" 
        read -r token
        case "$token" in
          ']') break ;;
          ',') ary="$ary," ;;
          *) error "EXPECTED , or ] GOT ${token:-EOF}";return 1 ;;
        esac
        read -r token
      done
      ;;
  esac
  value=
  :
}

function json_parse_object () {
  local key
  local obj=''
  read -r token
  case "$token" in
    '}') ;;
    *)
      while :
      do
        case "$token" in
          '"'*'"'|"'"*"'") key=$token ;;
          *) error "EXPECTED string GOT ${token:-EOF}";return 1 ;;
        esac
        read -r token
        case "$token" in
          ':') ;;
          *) error "EXPECTED : GOT ${token:-EOF}";return 1 ;;
        esac
        read -r token
        json_parse_value "$1" "$key" || return $?
        obj="$obj${key//\"/}:$value"        
        read -r token
        case "$token" in
          '}') break ;;
          ',') obj="$obj," ;;
          *) error "EXPECTED , or } GOT ${token:-EOF}";return 1 ;;
        esac
        read -r token
      done
    ;;
  esac
  value=
  :
}

function json_parse_value () {
  local jpath="${1:+$1.}$2" isleaf=0 isempty=0 print=0
  case "$token" in
    '{') json_parse_object "$jpath" || return $?;;
    '[') json_parse_array  "$jpath" || return $? ;;
    # At this point, the only valid single-character tokens are digits.
    ''|[!0-9]) error "EXPECTED value GOT ${token:-EOF}";return 1 ;;
    *) value=$token
       # if asked, replace solidus ("\/") in json strings with normalized value: "/"
       [ "$OPT_NORMALIZE_SOLIDUS" -eq 1 ] && value=$(echo "$value" | sed 's#\\/#/#g')
       isleaf=1
       [ "$value" = '""' ] && isempty=1
       ;;
  esac
  [ "$value" = '' ] && return

  printf "%s\t%s\n" "${jpath//[\"\']/}" "$value"
  :
}

function json_parse () {
  read -r token
  json_parse_value || return $?
  read -r token
  case "$token" in
    '') ;;
    *) error "EXPECTED EOF GOT $token";return 1 ;;
  esac
}
#json_tokenize "$1" | json_parse || exit $?
#exit

declare -A FILTER
FILTER=()
COLUMN=()

while true; do
	case $1 in
		-f|--filter)
			arg_check "$1" "$2"
			arg_filter "$2"
			shift;shift;;
		-c|-col|--column|--col)
			arg_check "$1" "$2"
			arg_column "$2"
			shift;shift;;
		-h|--help)
			print_help
			exit 0
			shift;;
		--)
			shift
			break;;
		-*)
			echo "未知选项 -- '$1'"
			print_help
			shift
			exit 2;;
		*)
			break;;
	esac
done

#echo ${FILTER[*]};exit;
#echo ${COLUMN[*]};exit;
FILE="$@"

if [[ "x$FILE" = "x" ]]; then
	while read line; do
		format "$line"
	done
else
	for file in "$FILE"; do
		if [[ -f "$file" ]]; then
			while read line; do
				format "$line"
			done < "$file"
		fi
	done 
fi
