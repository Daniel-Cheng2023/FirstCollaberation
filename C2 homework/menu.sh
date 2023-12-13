#!/bin/bash

prompt() {
	read -p "是否继续...[y/n] " res
	[ "$res" = "y" -o "$res" = "Y" ] && return 0 || return 1
}

# 定义主程序函数
main() {
	OPTION=$(whiptail --title " 信息集散中心 Version : 0.0.1 " --menu "
	作者：郑宏辉 陈天添
	请选择操作：" 25 60 5 \
	    "1" "登录/重新登录" \
	    "2" "读取信息" \
	    "3" "发送信息" \
	    "4" "退出" \
	    3>&1 1>&2 2>&3)

	# 按了取消 cancel
	[ $? -ne 0 ] && exit 1

	echo "OPTION: $OPTION"

	case "$OPTION" in
	    1 )
		    echo "登录..."
		    ./bignumber
		    prompt && main
		    ;;
	    2 )
		    echo "读取信息..."
		    prompt && main
		    ;;
	    3 )
		    echo "发送信息..."
		    prompt && main
		    ;;
	    4 )
		    echo "退出"
		    ;;
	esac
}

# 运行主程序
main

