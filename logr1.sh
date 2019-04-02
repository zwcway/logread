#!/bin/bash
#set -x

# 内置列：日志时间
COL_TIME=${COL_TIME:-t}
# 内置列：错误等级
COL_LEVEL=${COL_LEVEL:-l}
# 内置列：错误文件
COL_FILE=${COL_FILE:-f}
# 内置列：其他
COL_EXTRA=${COL_EXTRA:-e}

# 字段名称规则
REG_NAME='0-9a-zA-Z_'

# 开启转义符解析
OPT_NORMALIZE_SOLIDUS=${OPT_NORMALIZE_SOLIDUS:-0}
# 开启详细输出
OPT_BRIEF=${OPT_BRIEF:-1}

# 颜色配置
# 内置：
#	bg(background，全局背景颜色)
#	ky(key，字段名的颜色)
#	vl(value，值的颜色)
#	eq(=，符号的颜色)
# *开头：指定字段名称的颜色，使用英文句号分割json路径名。逗号分隔字段值，指定字段值的颜色。
#  		例如：*l,warning=93  表示当日志错误等级(t)为warning时，将整个字段置为亮黄色。
LOGR_COLORS=${LOGR_COLORS:-'ky=96:vl=90:*l,=97:*l,WARNING=93:*l,FATAL=91:*time=36:*time.ps_invoke=94:*errno,=91:*errno,0=90'}


HAS_SED=true

function print_help() {
	echo "用法: logr [参数]... [文件]..."
	echo "格式化日志"
	echo ""
	echo "  -h, --help           显示帮助内容"
	echo
	echo "  -c, --column         过滤列。多个列使用英文逗号分隔。内置列："
	echo "	                     time(日志生成时间) : $COL_TIME"
	echo "	                     level(日志等级)   : $COL_LEVEL"
	echo "	                     file(来源文件)    : $COL_FILE"
	echo "	                     extra(其他)      : $COL_EXTRA"
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
	echo
	echo '环境变量：'
	echo '   LOGR_COLORS'
}

function terminal() {
	echo "$1"
	print_help
	exit ${2:=1}
}

function error() {
  [ $OPT_BRIEF -eq 1 ] && echo "$*" >&2
}

function color_parser() {
	local opt key col color=$LOGR_COLORS
	
	( [[ -z "$LOGR_COLORS" ]] || [[ "$LOGR_COLORS" = *\ * ]] ) && return 1
	
	while :; do
		opt=${color%%:*}
		key=${opt%%=*}
		col=${opt#*=}
		( [[ -z "$opt" ]] || [[ -z "$key" ]] || [[ -z "$col" ]] ) && continue
		
		if [[ "${key:0:1}" = '*' ]]; then
			COLORS_VAL[${key:1}]='\e['${col}'m'
		else 
			COLORS_NAL[$key]='\e['${col}'m'
		fi
		
		col=${color#*:}
		[[ "$color" = "${col}" ]] && break
		color=$col
	done	
}

function print_field() {
	# 键/值 
	if [[ -n "${COLORS_VAL[$1,$2]}" ]]; then
		echo -ne ${COLORS_VAL[$1,$2]}
		echo -n "$1=$2 "
		echo -ne "\e[0m"
		return
	fi
	# 键/ 
	if [[ -n "${COLORS_VAL[$1,]}" ]]; then
		echo -ne ${COLORS_VAL[$1,]}
		echo -n "$1=$2 "
		echo -ne "\e[0m"
		return
	fi
	# 键
	if [[ -n "${COLORS_VAL[$1]}" ]]; then
		echo -ne ${COLORS_VAL[$1]}
		echo -n "$1"
		echo -ne "\e[0m"
	elif [[ -n "${COLORS_NAL[ky]}" ]]; then
		echo -ne ${COLORS_NAL[ky]}
		echo -n "$1"
		echo -ne "\e[0m"
	else 
		echo -n "$1"
	fi
	# =
	if [[ -n "${COLORS_NAL[eq]}" ]]; then
		echo -ne ${COLORS_NAL[eq]}
		echo -n '='
		echo -ne "\e[0m"
	else
		echo -n '='
	fi
	# 值
	if [[ -n "${COLORS_NAL[vl]}" ]]; then
		echo -ne ${COLORS_NAL[vl]}
		echo -n $(decode "$2")
		echo -ne "\e[0m"
	else
		echo -n "$2"
	fi
	echo -n ' '
}

function print_log() {
	local nodata=true tmp
	local head="$1"
	if filter; then
		if [[ ${#COLUMN[@]} -eq 0 ]]; then
			print_field "$COL_LEVEL" "${LINE_LOG[$COL_LEVEL]}"
			print_field "$COL_TIME" "${LINE_LOG[$COL_TIME]}"
			print_field "$COL_FILE" "${LINE_LOG[$COL_FILE]}"
			
			unset LINE_LOG[$COL_LEVEL]
			unset LINE_LOG[$COL_TIME]
			unset LINE_LOG[$COL_FILE]
			
			for key in "${!LINE_LOG[@]}"; do
				print_field "${key}" "${LINE_LOG[$key]}"
			done
			echo
		else
			for key in "${COLUMN[@]}"; do
				tmp=${key%%\.*}
				if [[ "$key" = "$tmp" ]]; then # 不用解析json
					[[ -n "${LINE_LOG[$key]}" ]]  && print_field "${key}" "${LINE_LOG[$key]}" && nodata=false
				else # 需要解析json
					[[ -n "${LINE_LOG[$tmp]}" ]] && format_json "${LINE_LOG[$tmp]}" "$tmp"
					[[ -n "${LINE_JSON[$key]}" ]] && print_field "${key}" "${LINE_JSON[$key]}" && nodata=false
				fi
			done
			# 换行
			$nodata || echo
		fi
	fi
}

function decode() {
	if $HAS_SED && [[ "$1" = *%* ]]; then
		if [[ "$1" = *FROM* ]]; then
			printf $(echo -n "$1" | sed 's/\\/\\\\/g;s/\(%\)\([0-9a-fA-F][0-9a-fA-F]\)/\\x\2/g') | sed 's/\+/ /g'
		else 
			printf $(echo -n "$1" | sed 's/\\/\\\\/g;s/\(%\)\([0-9a-fA-F][0-9a-fA-F]\)/\\x\2/g')
		fi
		return
	fi
	
	echo -n "$1"
}

function format() {
	LINE_LOG=()
	LINE_JSON=()
	LOADED_JSON=()
	local line="$*"
	local regral="((NOTICE|WARNING|TRACE): ([0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}):  ral-worker \* [0-9]+ \[([0-9a-zA-Z_/]+\.cpp:[0-9]+)\])(.*)"
	if [[ "$line" =~ $regral ]]; then
		format_ral "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}" "${BASH_REMATCH[3]}" "${BASH_REMATCH[5]}" "${BASH_REMATCH[4]}"
		[[ ${#LINE_LOG[@]} -gt 0 ]] && return 0
	fi
	local regapp="((WARNING|NOTICE|TRACE|DEBUG): ([0-9]{2}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}) \[(.+?:[0-9]+)\]) (.*)"
	if [[ "$line" =~ $regapp ]]; then
		format_app "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}" "${BASH_REMATCH[3]}" "${BASH_REMATCH[5]}" "${BASH_REMATCH[4]}"
		[[ ${#LINE_LOG[@]} -gt 0 ]] && return 0
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
	local val=''
	
	LINE_LOG["$COL_TIME"]="${log_time}"
	LINE_LOG["$COL_LEVEL"]="${log_level}"
	LINE_LOG["$COL_FILE"]="${log_file}"
	
	if [[ "$log_con" =~ $regral ]]; then
		log_con="${BASH_REMATCH[1]}"
		for json in $log_con; do
			val="${json#*=}"
			LINE_LOG["${json%%=*}"]="${val:= }"
		done
	fi
	print_log "$log_hed"
}

function format_app() {
	local log_hed="$1"
	local log_level="$2"
	local log_time="$3"
	local log_con="$4"
	local log_file="$5"
# TODO 配对匹配 json 中的 中括号，防止json中包含空格
	local regjson="([$REG_NAME]+)\[([^ ]+?)\] (.*)\$"
	
	LINE_LOG["$COL_TIME"]="${log_time}"
	LINE_LOG["$COL_LEVEL"]="${log_level}"
	LINE_LOG["$COL_FILE"]="${log_file}"
	
	while [[ "$log_con" =~ $regjson ]]; do
		log_con="${BASH_REMATCH[3]}"
		LINE_LOG["${BASH_REMATCH[1]}"]="${BASH_REMATCH[2]}"
	done
	
	if [[ -n "$log_con" ]]; then
		LINE_LOG["$COL_EXTRA"]="$log_con"
	fi
	print_log "$log_hed"
}

function format_json() {
	[[ "${LOADED_JSON[$2]}" ]] && return 0
	
	( [[ ${1:0:1} = '{' ]] || [[ ${1:0:1} = '[' ]] ) || return 1
	
	while read -r obj; do
		LINE_JSON["$2.${obj%%	*}"]="${obj#*	}"
	done <<<"$(json_tokenize "$1" $2 | json_parse)"
	
	LOADED_JSON[$2]=1
}

function filter() {
	local opr expr val name isJson=false
	for key in ${!FILTER[*]}; do
		name="$key"
		if [[ "$name" =~ \. ]]; then
			# json
			isJson=true
			name="${key%%\.*}"			
		fi
		
		
		if [[ "${LINE_LOG[$name]}" ]]; then
			val="${LINE_LOG[$name]}"
			if $isJson; then
				format_json "${LINE_LOG[$name]}" "$name"
				[[ "${LINE_JSON[$key]}" ]] && val="${LINE_JSON[$key]}"
			fi

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
		
		# TODO 高亮被过滤的字段
		#LINE_LOG[$name]=$(echo "${LINE_LOG[$name]}" | grep --color=always "$val")
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
	local regarg="^[$REG_NAME\.]+\$"
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
    # 只允许数字字符
    ''|[!0-9]) error "EXPECTED value GOT ${token:-EOF}";return 1 ;;
    *) value=$token
       # 如果设置了，将替换转义符(”\/“)为”/“
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

function env_check() {
	[[ -x /bin/sed ]] && HAS_SED=true || HAS_SED=false
}

declare -A FILTER
declare -A COLORS_NAL
declare -A COLORS_VAL
COLORS_NAL=()
COLORS_VAL=()
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
declare -A LINE_LOG
declare -A LINE_JSON
declare -A LOADED_JSON

color_parser
env_check

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
