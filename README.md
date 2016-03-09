# Qt Meta Object With Cef

This is a sample project that integrates Qt's Meta Object System with CEF. (Only Tested under Linux)

## Features

   * Supports passing any JSON stringifiable JS value to C++, namely
     - number (C++ number types(int, uint, float, double, etc), or the corresponding C++ enum member)
     - string (QString)
     - null   (empty QVariantMap)
     - undefined (empty QVariantMap)
     - array (QVariantList, or QStringList)
     - boolean (C++ bool)
     - object (QVariantMap)

   * Supports overloaded Qt slots.

   * Requires greater type strictness than Qt WebKit; not as many implicit-cast as Qt WebKit.

     For example, Javascript number `1` is not considered C++ boolean `true`, and such attempt will result in promise's rejection on the JS side.
     However if this behavior is very desired, you can implement it in `cg.js` fairly easily.

   * Wraps Qt Slot calls with ES6 Promises.

   * Wraps Qt Signal emissions with event handlers.

   * C++ enums are partially supported. See Limitations below.

## Limitations

   * The webview's `bridge`(or `channel` you may call), must be able to be compiled alone, without the WebView. This is
   because in this implementation, the `bridge`'s meta information is determined before compile-time, instead of run-time.

   * You'll need to manually name all relevant C++ enums in `cg.js`.

   * Class forward cannot be used in `bridge.h`, if you have a QObject under the entrance `Bridge` object.

   * Assumes all Qt slots' invocation happens on the CEF's BrowserProcess.

## How to use

   1. Generate a file describing `bridge`'s structure.

      Compile CMake target `QtMoExtractor`, and run

      ```
      mkdir QtSample/cg
      ./QtMoExtractor > QtSample/cg/Bridge.def
      ```

   2. Generate the codes that bridges Qt and CEF

      ```
      cd QtSample
      node --harmony --harmony_destructuring ./cg.js
      ```

   3. Compile CMake target `Application`

   4. Run

      ```
      ./Application --no-sandbox
      ```

## Requirements

   * CEF >= Branch 2623, equivalent to Chrome 49
   * C++14 compiler (With a few tweaks, this can be relaxed to C++11)
   * Recent versions of NodeJS
   * Qt5

## License

Do whatever you want with it.
