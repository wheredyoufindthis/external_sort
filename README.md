# TL;DR
`./build_and_test.sh && ./functional_test.sh`

## external_sort

* Есть функция сортировки (https://github.com/whereyoufindthis/external_sort/blob/master/external_sort/include/external_sort/external_sort.h)
  * Функция `Sort` написана так, что она не знает о реализации потоков выводов
  * Логика покрыта тестами (https://github.com/whereyoufindthis/external_sort/blob/master/external_sort/tests/main.cpp)

* Бинарный файл (https://github.com/whereyoufindthis/external_sort/blob/master/external_sort/bin/main.cpp)
  * Написана имлементация интерфейсов, нужных для работы функции `Sort`. Эти сущности позволяют явным образом контролировать размер используемой оперативной памяти
  * Там же определяются константы, которые будут фигурировать ниже: `CHUNK_SIZE`, `INPUT_FILE_BUFFER`, `OUTPUT_FILE_BUFFER`, `INPUT_MERGE_FILE_BUFFER` и `OUTPUT_CHUNK_FILE_BUFFER`
  
* Сортировка делится на два этапа:
  * Независимая сортировка `CHUNK_QTY = INPUT_FILE_SIZE / CHUNK_SIZE` кусков размера `CHUNK_SIZE` и запись каждого куска в отдельный временный файл
    * Потребление памяти - `max(INPUT_FILE_BUFFER, OUTPUT_CHUNK_FILE_BUFFER) + CHUNK_SIZE`
  * Оперативная память делится на `CHUNK_QTY + 1` кусков и делается `k-way merge` с записью в выходной файл
    * Первые `CHUNK_SIZE` кусков используются для входного буфера временных файлов, записанных на этапе 1
    * Последний кусок используется для буффера выходного файла
    * Потребление памяти - `INPUT_MERGE_FILE_BUFFER * CHUNK_QTY + OUTPUT_FILE_BUFFER`
  
## Производительность
* Два прохода по файлу
* Ассимптотика - `O(nlogn)`, где `n` - размер файла
  
## Особенности
* Только ASCII строчки, один символ = 1 байт
* Максимальная длина строки не должна превышать размеров всех буфферов и `CHUNK_SIZE`
* Пустые строчки скипаются
* Входной файл должен заканчиваться с `\n`
  
## Futher improvements
* Подумать над использованием `radix_sort` на 1 этапе работы программы, но там будет больше константа в ассимптотике и понадобится доп память
* Параллелизация сортировки на 1м этапе, но придется уменьшить размер чанка, чтобы все влезло в память
* Плохая производительность при большом кол-ве чанков:
  * Буффер чтения чанков будет маленьким. Надо будет добавлять еще несколько этапов `k-way merge` для сливания мелких чанков
