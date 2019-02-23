# H1

## H2

### H3

Multi-line paragraph bla bla bla
bla bla bla.

Intentional line break  
via two spaces at line.

Formats: _italic_, **bold**, `monospace`, ~~strikeout~~

Bullet list:

* item1
* item2

Numbered list:

1. item 1
2. item 2

[link](http://kde.org)

    code 1
    code 2

normal text

> block quote _italic_
> more block quote

normal text

Title: some text

normal text

# Fenced code blocks (bug 356974)

## Bash

```bash
#!/usr/bin/env bash

for f in *; do
    echo "$f"
done
```

## C++

```cpp
#include <QtCore/QObject>

class Q : public QObject {
Q_OBJECT
public:
    Q();
}

Q::Q() :QObject(NULL) {
}
```

## CMake

```cmake
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project (hello_world)

set(QT_MIN_VERSION "5.6.0")
```

## CSS

```css
h1.main::hover {
    font-size: 100;
    color: green;
}
```

## Email

```email
From: Konqi Dragon <konqi@kde.org>
To: All <all@kde.og>
Subject: highlights
```

## Haskell

```haskell
module Main (main) where

main :: IO ()
main = putStrLn "Hello, World!"
```

## HTML

```html
<html>
  <body>
    <p class="main">Hello world!</p>
  </body>
</html>
```

## JSON

```json
[{
    "hello": "world",
    "count": 1,
    "bool": true
}]
```

## KConfig

```kconfig
[General]
Toggle=true
Count=1
```

## PHP

```php
namespace Application\Controller;

use Zend\Mvc\Controller\AbstractActionController;

class IndexController extends AbstractActionController
{
}
```

## Python

```python
def addXToY(x, y):
    total = x + y
    print total
```

## QML

```qml
Text {
    id: hello
    width: 100
    text: "Hello world!"
}
```

## Rust

```rust
fn main() {
    println!("Hello world!");
}
```

## XML

```xml
<tag attribute="3">
  <nested/>
</tag>
```

## No language specified

```
No language is specified, but it should be still rendered as code block.
```
