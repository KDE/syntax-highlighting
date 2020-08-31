// Vala Sample File

/*
 * Multiline comment
 * NOTE FIXME ALERT
 */
void main() {
    print("Hello World\n");
}

// Example 1
class Sample: Object {
    void greeting() {
        stdout.printf("Hello World\n");
    }

    static void main(string[] args) {
        var sample = new Sample();
        sample.greeting();
    }
}

// Example 2
using GLib;

interface Printable {
    public abstract string print();

    public virtual string pretty_print() {
        return "Please " + print();
    }
}

class NormalPrint: Object, Printable {
    string print() {
        return "don't forget about me";
    }
}

class OverridePrint: Object, Printable {
    string print() {
        return "Mind the gap";
    }

    public override string pretty_print() {
        return "Override";
    }
}

void main(string[] args) {
    var normal = new NormalPrint();
    var overridden = new OverridePrint();

    print(normal.pretty_print());
    print(overridden.pretty_print());
}

// Example 3
using Gtk;

int main(string[] args) {
    Gtk.init(ref args);

    var window = new Window();
    window.title = "Hello, World!";
    window.border_width = 10;
    window.window_position = WindowPosition.CENTER;
    window.set_default_size(350, 70);
    window.destroy.connect(Gtk.main_quit);

    var label = new Label("Hello, World!");

    window.add(label);
    window.show_all();

    Gtk.main();
    return 0;
}
