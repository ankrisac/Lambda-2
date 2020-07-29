clear

case $1 in
    mem)
        make build && valgrind --leak-check=yes ./.bin/main
    ;;
    run)
        make build && ./.bin/main
    ;;
    release)
        make $1 && ./.bin/release
    ;;
    debug)
        make $1
    ;;
    web)
        make $1
    ;;
esac
