#version 330 core

// single line comment

/* single line commonet */

/*
 multi line comment
 */

in ColorFormat {
    vec3 color;
} fs_in;

out vec4 fragColor;

void main()
{
    fragColor = vec4( fs_in.color, 1.0 );
}
