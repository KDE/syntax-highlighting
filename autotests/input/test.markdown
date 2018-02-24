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

## No language specified

```
No language is specified, but it should be still rendered as code block.
```
