import 'dart:io' show File;
/**
 * print a number
 * "helo"
 * 'string'
 */

const List<int> list = [1];

String s = '''
Multline string single quote
''';

String d = '''
Multline string double quote
''';

enum Enum { one, two }

class SimpleClass {}

abstract class Interface extends SimpleClass {}

final class Dummy {}

Stream<int> gen() async* {
  for (int i = 0; i < 100; ++i) {
    yield switch (i) {
      1 => 1,
      _ => throw "Invalid",
    };
    yield i;
  }
}

Future<int> printNum(int anum) async {
  print("This is a $anum\n");
  print('This is a $anum\n \'world\'');
  if (anum == 1) {
    switch (anum) {
      case 1:
        return 3;
      default:
        // do while
        do {
          anum--;
        } while (anum < 0);
        // while
        while (anum > 0) {
          anum--;
        }
        break;
    }
    return 1;
  } else {
    return 2;
  }
}

// What are you doing?
void main() {
  final int nn = 10;
  double dd = 1.5;
  double de = double.nan;
  bool boolean = true;
  printNum(nn);
}
