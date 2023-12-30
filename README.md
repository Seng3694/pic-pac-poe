# Pic Pac Poe

A command line tool for packing textures. A **pic**ture **pac**king [poe](https://zelda.fandom.com/wiki/Poe).

## Usage

Pipe png paths into the application and specify the packing script. Optionally specify the exporting script.

```sh
find . -name '*.png' | ppp -ps example/bin_pack.lua -es example/header_export.lua
```

This command will pack the images found into a single image using the `bin_pack.lua` script and exports a C header file with the `header_export.lua` script.

Use `--help` for more information.

```sh
ppp --help
```

## Example

Input textures:

![00](content/00.png)
![02](content/02.png)
![03](content/03.png)
![17](content/17.png)
![19](content/19.png)
![22](content/22.png)
![29](content/29.png)
![30](content/30.png)

```sh
find ./content -name '*.png' |\
    bin/ppp \  
        -ps example/bin_pack.lua \ 
        -es example/header_export.lua \ 
        -w 64 -po my_texture.png -eo my_texture.h
```

This generates the following texture:

![my_texture](content/my_texture.png)

(Note: if you run this command then make sure to filter or delete the already existent `my_texture.png`)

and exports a C header file:

```c
#ifndef MY_TEXTURE_H
#define MY_TEXTURE_H

typedef enum {
  MY_TEXTURE_ID_00,
  MY_TEXTURE_ID_02,
  MY_TEXTURE_ID_03,
  MY_TEXTURE_ID_17,
  MY_TEXTURE_ID_19,
  MY_TEXTURE_ID_22,
  MY_TEXTURE_ID_29,
  MY_TEXTURE_ID_30
} my_texture_id;

struct {
  int left, top, width, height;
} const static my_texture_rects[] = {
  { 32, 96, 8, 8 },
  { 40, 96, 8, 8 },
  { 32, 104, 8, 8 },
  { 16, 96, 16, 16 },
  { 0, 96, 16, 16 },
  { 0, 64, 32, 32 },
  { 32, 64, 32, 32 },
  { 0, 0, 64, 64 },
};

#endif

```

## Packing Lua API

```lua
function Pack(rectangles, width)
    -- 'rectangles' is an unsorted array with rectangle tables
    -- each rectangle has the following fields
    --   left      (unset)
    --   top       (unset)
    --   width
    --   height
    --   textureId (only meant for internal use)

    -- the 'width' parameter is a constraint for the output image

    -- this function should return the rectangles array with set 'left' and 'top'
    -- followed by the width and height of the output image
    return rectangles, width, 123
end
```

See the [example](example/bin_pack.lua) for more information.

## Exporting Lua API

```lua
function Export(textures, outputFile)
    -- 'textures' is an unsorted array with texture tables
    -- each texture has the following fields
    --   left
    --   top
    --   width
    --   height
    --   name (file path which was originally piped into ppp)

    -- 'outputFile' is a string containing the file path which should be used to write the outputs to
end
```

See the [example](example/header_example.lua) for more information.

## Notes

- ppp requires input from stdin when not using `--help`
- ppp only works with png files

## Build and install

```sh
git clone https://github.com/seng3694/pic-pac-poe
cd pic-pac-poe
sudo make install release=1
```

## Uninstall

```sh
sudo make uninstall
```

## Dependencies

- [Lua](https://github.com/lua/lua)
- [stb_image](https://github.com/nothings/stb/stb_image.h)
- [stb_image_write](https://github.com/nothings/stb/stb_image_write.h)

## License

[MIT License](LICENSE)
