# Как собрать
Сборка с помощью CMake.
Запускаем в терминале
### Для Windows
```
  cd "Путь\папки\проекта"
  mkdir build
  cd build
  cmake ..
  cmake --build .
```
# Как использовать
GUI:
* В секции `Входной файл` содержится поле для ввода имени входного файла и галочка для удаления входного файла после успешного выполнения программы. В поле поддерживается любое расширение и указание полного пути файла.
---
* В секции `Выходной файл` содержится поле для ввода полного пути выходного файла и галочки связанные перезаписью, созданием копии выходного файла. Поле ввода поддерживает указание полного пути, где создать файл, в этом случаем будет создан выходной файл с названием masked_file.txt, поддерживает указание полного пути файла. При наличии одноименного выходного файла можно использовать галочки для перезаписи одноименного файла и для создании копии с цифрой. Если ни одна галочка не выбрана, то работает режим добавления результата программы в одноименный файл.
---
* В оставшейся секции можно указать значение для бинарной операции и операцию преобразования и включить режим таймера. В этом режиме программа будет каждый утановленный интервал преобразовывать входной файл и создавать файл результата. Чтобы запустить таймер, нужно нажать на центральную кнопку. Чтобы остановить, необходимо нажать повторно.
