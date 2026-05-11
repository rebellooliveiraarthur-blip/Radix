from PIL import Image
import sys

def rgb888_to_rgb565(r, g, b):
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)

def image_to_sprite_c_array(image_path, sprite_name):
    img = Image.open(image_path).convert('RGBA')
    width, height = img.size

    pixels = []
    for y in range(height):
        row = []
        for x in range(width):
            r, g, b, a = img.getpixel((x, y))
            if a < 128:
                row.append("____")
            else:
                row.append(f"0x{rgb888_to_rgb565(r, g, b):04X}")
        pixels.append(row)

    lines = [f"// Sprite: {sprite_name}  ({width}x{height})"]
    lines.append(f"Color pixels_{sprite_name}[] = {{")
    for y, row in enumerate(pixels):
        prefix = "    " if y == 0 else "    "
        suffix = "," if y < height - 1 else ""
        lines.append(f"{prefix}{', '.join(row)}{suffix}")
    lines.append("};")
    lines.append("")
    lines.append(f"Sprite {sprite_name} = {{ {width}, {height}, pixels_{sprite_name} }};")
    return "\n".join(lines) + "\n"

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Uso: {sys.argv[0]} <imagem_entrada> <nome_sprite> [arquivo_saida]")
        print(f"Ex:  {sys.argv[0]} background.png background assets/background/background_sprite.h")
        sys.exit(1)

    input_path = sys.argv[1]
    sprite_name = sys.argv[2]
    output_path = sys.argv[3] if len(sys.argv) > 3 else f"{sprite_name}_sprite.h"

    c_code = image_to_sprite_c_array(input_path, sprite_name)

    with open(output_path, "w") as f:
        f.write(c_code)

    print(f"Sprite '{sprite_name}' salvo em: {output_path}")
    print(f"Dimensoes: {Image.open(input_path).size}")
