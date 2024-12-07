#!/bin/bash

# Функция для поиска всех библиотек и вывод их версий
find_libraries() {
    echo "Поиск библиотек и их версий в системе..."
    for lib in $(find / -type f \( -name "*.so" -o -name "*.so.*" \) 2>/dev/null); do
        echo "Библиотека: $lib"
        version=$(strings $lib 2>/dev/null | grep -E "^[0-9]+\.[0-9]+(\.[0-9]+)?$")
        if [[ ! -z $version ]]; then
            echo "Версия: $version"
        else
            echo "Версия не найдена"
        fi
        echo "-----------------------"
    done
}

# Запуск функции
find_libraries

