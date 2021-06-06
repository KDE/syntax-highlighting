/**
 * print a number
 * "helo"
 * 'string'
 */

int printNum(int anum) {
    print("This is a $anum\n");
    print('This is a $anum\n \'world\'');
    if (anum == 1) {
        return 1;
    } else {
        return 2;
    }
}

// What are you doing?
void main() {
    int nn = 10;
    bool boolean = true;
    printNum(nn);
}
