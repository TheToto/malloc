#!/bin/bash

show_res() {
    echo -e "\033[1;4mLaunch $1 binary whith LD_PRELOAD\033[0m"
    echo
    time LD_PRELOAD=./libmalloc.save.so $1
    ret=$?
    seg="\033[0m"
    [ "$ret" -eq 139 ] && seg="\033[31;1;4;5;7mSEGFAULT !\033[0m"
    if [ $ret == 0 ]; then
        echo -e "\033[32mExit status : $ret $seg"
    else
        echo -e "\033[31mExit status : $ret $seg"
    fi
    if [ "0" == "$2" ]; then
        echo
        echo -e "\033[1;4mReal malloc result :\033[0m"
        time $1 > tmp2
        seg="\033[0m"
        [ "$ret" -eq 139 ] && seg="\033[31;1;4;5;7mSEGFAULT !\033[0m"
        if [ $ret == 0 ]; then
            echo -e "\033[32mExit status : $ret $seg"
        else
            echo -e "\033[31mExit status : $ret $seg"
        fi
        rm tmp2
    fi
    [ -f "my_test" ] && rm my_test
    echo
    echo -e "\033[4mPress Enter to continue...\033[0m"
    read
}

show_test() {
    clear
    echo
    echo -e "\033[1;4mChoose a test: (put .c files in tests/src folder or type a binary)\033[0m"
    i=1
    for file in $(echo tests/src/*.c)
    do
        echo "$i - $file"
        ((i++))
    done
    echo
    read res
    i=1
    [ -z "$res" ] && return
    for file in $(echo tests/src/*.c)
    do
        if [ "$res" -eq "$i" ]; then
            clear
            echo "Compiling $file..."
            gcc "$file" -o my_test
            echo "Compiled !"
            echo
            show_res "./my_test" "0"
            show_test
        fi
        ((i++))
    done
}

show_binary() {
    clear
    echo
    echo -e "\033[1;4mChoose a binary: (you can edit tests/config_bin file or type a binary)\033[0m"
    i=1
    while read file
    do
        echo "$i - $file"
        ((i++))
    done < tests/config_bin
    echo
    read res
    i=1
    [ -z "$res" ] && return
    re='^[0-9]+([.][0-9]+)?$'
    if ! [[ "$res" =~ $re ]] ; then
        show_res "$res" "0"
        return
    fi
    while read file
    do
        if [ "$res" -eq "$i" ]; then
            clear
            exe="$file"
        fi
        ((i++))
    done < tests/config_bin
    clear
    echo
    show_res "$exe" 0
    show_binary
}

show_gui() {
    clear
    echo
    echo -e "\033[1;4mChoose a binary: (you can edit tests/config_bin file)\033[0m"
    i=1
    for file in $(cat tests/config_gui)
    do
        echo "$i - $file"
        ((i++))
    done
    echo
    read res
    i=1
    [ -z "$res" ] && return
    re='^[0-9]+([.][0-9]+)?$'
    if ! [[ "$res" =~ $re ]] ; then
        show_res "$res" "1"
        return
    fi
    for file in $(cat tests/config_gui)
    do
        if [ "$res" -eq "$i" ]; then
            clear
            show_res "$file" "1"
            return
        fi
        ((i++))
    done
}


show_menu() {
    clear
    echo
    echo -e "\033[1;4mChoose an options:\033[0m"
    echo "1 - Launch one of my tests"
    echo "2 - Launch a console binary"
    echo "3 - Launch a GUI binary"
    echo "4 - Quit"
    echo
    read res
    [ -z "$res" ] && res=4
    case "$res" in
        1)
            show_test
            show_menu
            ;;
        2)
            show_binary
            show_menu
            ;;
        3)
            show_gui
            show_menu
            ;;
        4)
            rm libmalloc.save.so
            exit 0
            ;;
        *)
            show_menu
            ;;
    esac
}
cp libmalloc.so libmalloc.save.so
show_menu
