// @Utility shader

#version 450

vec4 overrideColorGreen(vec4 originalColor) {
    // You coulssd apply blending/tinting here if you prefer
    return vec4(0.0, 1.0, 0.0, originalColor.a);
}