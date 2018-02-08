/**********************************************\
*
*  Andrey A. Ugolnik
*  http://www.ugolnik.info
*  andrey@ugolnik.info
*
\**********************************************/

#include "common/bitmap_description.h"
#include "common/file.h"
#include "common/helpers.h"
#include "types/types.h"

#include <algorithm>
#include <memory>
#include <vector>

enum xcf_prop_type
{
    PROP_END = 0,
    PROP_COLORMAP = 1,
    PROP_ACTIVE_LAYER = 2,
    PROP_ACTIVE_CHANNEL = 3,
    PROP_SELECTION = 4,
    PROP_FLOATING_SELECTION = 5,
    PROP_OPACITY = 6,
    PROP_MODE = 7,
    PROP_VISIBLE = 8,
    PROP_LINKED = 9,
    PROP_PRESERVE_TRANSPARENCY = 10,
    PROP_APPLY_MASK = 11,
    PROP_EDIT_MASK = 12,
    PROP_SHOW_MASK = 13,
    PROP_SHOW_MASKED = 14,
    PROP_OFFSETS = 15,
    PROP_COLOR = 16,
    PROP_COMPRESSION = 17,
    PROP_GUIDES = 18,
    PROP_RESOLUTION = 19,
    PROP_TATTOO = 20,
    PROP_PARASITES = 21,
    PROP_UNIT = 22,
    PROP_PATHS = 23,
    PROP_USER_UNIT = 24
};

struct xcf_prop
{
    uint32_t id;
    uint32_t length;
    union
    {
        struct
        {
            uint32_t num;
            char* cmap;
        } colormap; // 1
        struct
        {
            uint32_t drawable_offset;
        } floating_selection; // 5
        int32_t opacity;
        int32_t mode;
        int32_t visible;
        int32_t linked;
        int32_t preserve_transparency;
        int32_t apply_mask;
        int32_t show_mask;
        struct
        {
            int32_t x;
            int32_t y;
        } offset;
        uint8_t color[3];
        uint8_t compression;
        struct
        {
            int32_t x;
            int32_t y;
        } resolution;
        struct
        {
            char* name;
            uint32_t flags;
            uint32_t size;
            char* data;
        } parasite;
    } data;
};

enum xcf_compr_type
{
    COMPR_NONE = 0,
    COMPR_RLE = 1,
    COMPR_ZLIB = 2,
    COMPR_FRACTAL = 3
};

struct xcf_header
{
    char sign[14];
    uint32_t width;
    uint32_t height;
    int32_t image_type;
    std::vector<xcf_prop> properties;

    std::vector<uint32_t> layer_file_offsets;
    std::vector<uint32_t> channel_file_offsets;

    xcf_compr_type compr;
    std::vector<uint8_t> cm_map;
};

struct xcf_layer
{
    uint32_t width;
    uint32_t height;
    int32_t layer_type;
    std::string name;
    std::vector<xcf_prop> properties;

    uint32_t hierarchy_file_offset;
    uint32_t layer_mask_offset;

    uint32_t offset_x;
    uint32_t offset_y;
    int32_t visible;
};

template <typename T>
T fread(cFile& file, bool big_endian)
{
    T res{};
    file.read(reinterpret_cast<char*>(&res), sizeof(T));
    if (big_endian)
    {
        auto beg = reinterpret_cast<char*>(&res);
        std::reverse(beg, beg + sizeof(T));
    }
    return res;
}

void xcf_read_property(cFile& file, xcf_prop* prop)
{
    prop->id = fread<uint32_t>(file, true);
    prop->length = fread<uint32_t>(file, true);

#if DEBUG
    // printf("%.8X: %s: %d\n", SDL_RWtell(src), prop->id < 25 ? prop_names[prop->id] : "unknown", prop->length);
#endif

    uint32_t len = 0;

    switch (prop->id)
    {
    case PROP_COLORMAP:
        prop->data.colormap.num = fread<uint32_t>(file, true);
        prop->data.colormap.cmap = new char[prop->data.colormap.num * 3];
        file.read(prop->data.colormap.cmap, prop->data.colormap.num * 3);
        break;

    case PROP_OFFSETS:
        prop->data.offset.x = fread<uint32_t>(file, true);
        prop->data.offset.y = fread<uint32_t>(file, true);
        break;

    case PROP_OPACITY:
        prop->data.opacity = fread<uint32_t>(file, true);
        break;

    case PROP_COMPRESSION:
    case PROP_COLOR:
        if (prop->length > sizeof(prop->data))
        {
            len = sizeof(prop->data);
        }
        else
        {
            len = prop->length;
        }
        file.read(&prop->data, len);
        break;

    case PROP_VISIBLE:
        prop->data.visible = fread<uint32_t>(file, true);
        break;

    default:
        //    SDL_RWread (src, &prop->data, prop->length, 1);
        file.seek(prop->length, SEEK_CUR);
    }
}

bool read_xcf_header(cFile& file, xcf_header& h)
{
    if (file.read(h.sign, sizeof(h.sign)) != sizeof(h.sign))
    {
        return false;
    }

    h.width = fread<uint32_t>(file, true);
    h.height = fread<uint32_t>(file, true);
    h.image_type = fread<uint32_t>(file, true);

    h.properties.reserve(100);
    h.layer_file_offsets.reserve(100);
    h.compr = COMPR_NONE;
    h.cm_map.reserve(100);

    // Just read, don't save
    xcf_prop prop;

    do
    {
        xcf_read_property(file, &prop);
        if (prop.id == PROP_COMPRESSION)
        {
            h.compr = (xcf_compr_type)prop.data.compression;
        }
        else if (prop.id == PROP_COLORMAP)
        {
            uint32_t cm_num = prop.data.colormap.num;
            h.cm_map.resize(cm_num * 3);
            ::memcpy(h.cm_map.data(), prop.data.colormap.cmap, cm_num * 3);
        }
    } while (prop.id != PROP_END);

    return true;
}

uint8_t* load_xcf_tile_none(cFile& file, uint32_t len, int32_t bpp, int32_t x, int32_t y)
{
    auto load = new uint8_t[len];
    file.read(load, len);

    return load;
}

uint8_t* load_xcf_tile_rle(cFile& file, uint32_t len, int32_t bpp, int32_t x, int32_t y)
{
    std::unique_ptr<uint8_t[]> load(new uint8_t[len]);

    auto t = load.get();
    if (file.read(t, len) != len)
    {
        return nullptr;
    }

    auto data = new uint8_t[x * y * bpp];

    for (int32_t i = 0; i < bpp; i++)
    {
        auto d = data + i;
        int32_t size = x * y;
        int32_t count = 0;

        while (size > 0)
        {
            uint8_t val = *t++;

            int32_t length = val;
            if (length >= 128)
            {
                length = 255 - (length - 1);
                if (length == 128)
                {
                    length = (*t << 8) + t[1];
                    t += 2;
                }

                count += length;
                size -= length;

                while (length-- > 0)
                {
                    *d = *t++;
                    d += bpp;
                }
            }
            else
            {
                length += 1;
                if (length == 128)
                {
                    length = (*t << 8) + t[1];
                    t += 2;
                }

                count += length;
                size -= length;

                val = *t++;

                for (int32_t j = 0; j < length; j++)
                {
                    *d = val;
                    d += bpp;
                }
            }
        }
    }

    return data;
}

std::string read_string(cFile& file)
{
    auto tmp = fread<uint32_t>(file, false);
    if (tmp > 0)
    {
        std::vector<char> data(tmp + 1);
        file.read(data.data(), tmp);
        data[tmp] = 0;
        return std::string{ data.data() };
    }
    return {};
}

xcf_layer read_xcf_layer(cFile& file)
{
    xcf_layer l;
    l.width = fread<uint32_t>(file, true);
    l.height = fread<uint32_t>(file, true);
    l.layer_type = fread<uint32_t>(file, true);

    l.name = read_string(file);

    xcf_prop prop;

    do
    {
        xcf_read_property(file, &prop);
        if (prop.id == PROP_OFFSETS)
        {
            l.offset_x = prop.data.offset.x;
            l.offset_y = prop.data.offset.y;
        }
        else if (prop.id == PROP_VISIBLE)
        {
            l.visible = prop.data.visible ? 1 : 0;
        }
    } while (prop.id != PROP_END);

    l.hierarchy_file_offset = fread<uint32_t>(file, true);
    l.layer_mask_offset = fread<uint32_t>(file, true);

    return l;
}

typedef uint8_t* (*load_tile_type)(cFile&, uint32_t, int32_t, int32_t, int32_t);

struct xcf_hierarchy
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;

    std::vector<uint32_t> level_file_offsets;
};

xcf_hierarchy read_xcf_hierarchy(cFile& file)
{
    xcf_hierarchy h;
    h.width = fread<uint32_t>(file, true);
    h.height = fread<uint32_t>(file, true);
    h.bpp = fread<uint32_t>(file, true);

    h.level_file_offsets.reserve(100);
    do
    {
        auto offset = fread<uint32_t>(file, true);
        h.level_file_offsets.push_back(offset);
    } while (h.level_file_offsets.back() != 0);

    return h;
}

struct xcf_level
{
    uint32_t width;
    uint32_t height;

    std::vector<uint32_t> tile_file_offsets;
};

xcf_level read_xcf_level(cFile& file)
{
    xcf_level l;
    l.width = fread<uint32_t>(file, true);
    l.height = fread<uint32_t>(file, true);

    l.tile_file_offsets.reserve(100);
    do
    {
        auto offset = fread<uint32_t>(file, true);
        l.tile_file_offsets.push_back(offset);
    } while (l.tile_file_offsets.back() != 0);

    return l;
}

uint32_t Swap32(uint32_t v)
{
    return ((v & 0x000000FF) << 16)
        | ((v & 0x0000FF00))
        | ((v & 0x00FF0000) >> 16)
        | ((v & 0xFF000000));
}

enum xcf_image_type
{
    IMAGE_RGB = 0,
    IMAGE_GREYSCALE = 1,
    IMAGE_INDEXED = 2
};

bool do_layer_surface(sBitmapDescription& desc, cFile& file, xcf_header& head, xcf_layer& layer, load_tile_type load_tile)
{
    file.seek(layer.hierarchy_file_offset, SEEK_SET);
    auto hierarchy = read_xcf_hierarchy(file);

    for (uint32_t i = 0; hierarchy.level_file_offsets[i]; i++)
    {
        file.seek(hierarchy.level_file_offsets[i], SEEK_SET);
        auto level = read_xcf_level(file);

        uint32_t ty = 0;
        uint32_t tx = 0;
        for (uint32_t j = 0; level.tile_file_offsets[j]; j++)
        {
            file.seek(level.tile_file_offsets[j], SEEK_SET);
            uint32_t ox = tx + 64 > level.width ? level.width % 64 : 64;
            uint32_t oy = ty + 64 > level.height ? level.height % 64 : 64;

            std::unique_ptr<uint8_t[]> tile;

            if (level.tile_file_offsets[j + 1])
            {
                tile.reset(load_tile(file, level.tile_file_offsets[j + 1] - level.tile_file_offsets[j], hierarchy.bpp, ox, oy));
            }
            else
            {
                tile.reset(load_tile(file, ox * oy * 6, hierarchy.bpp, ox, oy));
            }

            auto p8 = tile.get();
            auto p = (uint32_t*)p8;
            for (uint32_t y = ty; y < ty + oy; y++)
            {
                auto row = (uint32_t*)((uint8_t*)desc.bitmap.data() + y * desc.pitch + tx * 4);
                switch (hierarchy.bpp)
                {
                case 4:
                    for (uint32_t x = tx; x < tx + ox; x++)
                    {
                        *row++ = Swap32(*p++);
                    }
                    break;

                case 3:
                    for (uint32_t x = tx; x < tx + ox; x++)
                    {
                        *row = 0xFF000000;
                        *row |= ((uint32_t)*p8++ << 16);
                        *row |= ((uint32_t)*p8++ << 8);
                        *row |= ((uint32_t)*p8++ << 0);
                        row++;
                    }
                    break;

                case 2:
                    /* Indexed / Greyscale + Alpha */
                    switch (head.image_type)
                    {
                    case IMAGE_INDEXED:
                        for (uint32_t x = tx; x < tx + ox; x++)
                        {
                            *row = ((uint32_t)(head.cm_map[*p8 * 3]) << 16);
                            *row |= ((uint32_t)(head.cm_map[*p8 * 3 + 1]) << 8);
                            *row |= ((uint32_t)(head.cm_map[*p8++ * 3 + 2]) << 0);
                            *row |= ((uint32_t)*p8++ << 24);
                            row++;
                        }
                        break;

                    case IMAGE_GREYSCALE:
                        for (uint32_t x = tx; x < tx + ox; x++)
                        {
                            *row = ((uint32_t)*p8 << 16);
                            *row |= ((uint32_t)*p8 << 8);
                            *row |= ((uint32_t)*p8++ << 0);
                            *row |= ((uint32_t)*p8++ << 24);
                            row++;
                        }
                        break;

                    default:
                        ::printf("(EE) Unknown Gimp image type (%d)\n", head.image_type);
                        return false;
                    }
                    break;

                case 1:
                    /* Indexed / Greyscale */
                    switch (head.image_type)
                    {
                    case IMAGE_INDEXED:
                        for (uint32_t x = tx; x < tx + ox; x++)
                        {
                            *row++ = 0xFF000000
                                | ((uint32_t)(head.cm_map[*p8 * 3]) << 16)
                                | ((uint32_t)(head.cm_map[*p8 * 3 + 1]) << 8)
                                | ((uint32_t)(head.cm_map[*p8 * 3 + 2]) << 0);
                            p8++;
                        }
                        break;

                    case IMAGE_GREYSCALE:
                        for (uint32_t x = tx; x < tx + ox; x++)
                        {
                            *row++ = 0xFF000000
                                | (((uint32_t)(*p8)) << 16)
                                | (((uint32_t)(*p8)) << 8)
                                | (((uint32_t)(*p8)) << 0);
                            ++p8;
                        }
                        break;

                    default:
                        ::printf("(EE) Unknown Gimp image type (%d)\n", head.image_type);
                        return false;
                    }
                    break;
                }
            }

            tx += 64;
            if (tx >= level.width)
            {
                tx = 0;
                ty += 64;
            }
            if (ty >= level.height)
            {
                break;
            }
        }
    }

    return true;
}

struct xcf_channel
{
    uint32_t width;
    uint32_t height;
    std::string name;
    std::vector<xcf_prop> properties;

    uint32_t hierarchy_file_offset;

    uint32_t color;
    uint32_t opacity;
    int selection;
    int visible;
};

xcf_channel read_xcf_channel(cFile& file)
{
    xcf_channel l;
    l.width = fread<uint32_t>(file, true);
    l.height = fread<uint32_t>(file, true);
    l.name = read_string(file);

    l.selection = 0;

    xcf_prop prop;
    do
    {
        xcf_read_property(file, &prop);
        switch (prop.id)
        {
        case PROP_OPACITY:
            l.opacity = prop.data.opacity << 24;
            break;

        case PROP_COLOR:
            l.color = ((uint32_t)prop.data.color[0] << 16)
                | ((uint32_t)prop.data.color[1] << 8)
                | ((uint32_t)prop.data.color[2]);
            break;

        case PROP_SELECTION:
            l.selection = 1;
            break;

        case PROP_VISIBLE:
            l.visible = prop.data.visible ? 1 : 0;
            break;

        default:
            break;
        }
    } while (prop.id != PROP_END);

    l.hierarchy_file_offset = fread<uint32_t>(file, true);

    return l;
}

bool load(cFile& file, sBitmapDescription& desc)
{
    xcf_header head;
    if (read_xcf_header(file, head) == false)
    {
        ::printf("(EE) Can't read XCF header.\n");
    }

    uint8_t* (*load_tile)(cFile&, uint32_t, int32_t, int32_t, int32_t);

    switch (head.compr)
    {
    case COMPR_NONE:
        load_tile = load_xcf_tile_none;
        break;

    case COMPR_RLE:
        load_tile = load_xcf_tile_rle;
        break;

    default:
        ::printf("(EE) Unsupported Compression.\n");
        return false;
    }

    desc.size = file.getSize();
    desc.format = GL_RGBA;
    desc.bpp = 32;
    desc.bppImage = 32;
    desc.width = head.width;
    desc.height = head.height;
    desc.pitch = head.width * 4;
    desc.bitmap.resize(desc.pitch * head.height);

    auto pix = desc.bitmap.data();
    std::fill(desc.bitmap.begin(), desc.bitmap.end(), 0);

    head.layer_file_offsets.reserve(100);
    do
    {
        head.layer_file_offsets.push_back(fread<uint32_t>(file, true));
    } while (head.layer_file_offsets.back() != 0);

    auto fp = file.getOffset();

    // Blit layers backwards, because Gimp saves them highest first
    for (size_t i = 0, size = head.layer_file_offsets.size(); i < size; i++)
    {
        size_t idx = size - i - 1;
        file.seek(head.layer_file_offsets[idx], SEEK_SET);

        auto layer = read_xcf_layer(file);
        do_layer_surface(desc, file, head, layer, load_tile);

        // SDL_Rect rs;
        // rs.x = 0;
        // rs.y = 0;
        // rs.w = layer->width;
        // rs.h = layer->height;

        // SDL_Rect rd;
        // rd.x = layer->offset_x;
        // rd.y = layer->offset_y;
        // rd.w = layer->width;
        // rd.h = layer->height;

        // if (layer->visible)
        // {
        // SDL_BlitSurface(lays, &rs, surface, &rd);
        // }
    }

    file.seek(fp, SEEK_SET);

    // read channels
    std::vector<xcf_channel> channel;
    channel.reserve(100);

    while (true)
    {
        auto offset = fread<uint32_t>(file, true);
        if (offset == 0)
        {
            break;
        }

        auto fp = file.getOffset();
        file.seek(offset, SEEK_SET);

        channel.push_back(read_xcf_channel(file));

        file.seek(fp, SEEK_SET);
    }

    if (channel.size())
    {
#if 0
        SDL_Surface* chs = SDL_CreateRGBSurface(SDL_SWSURFACE, head.width, head.height, 32,
                                                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        if (chs == nullptr)
        {
            error = "Out of memory";
            goto done;
        }
        for (i = 0; i < chnls; i++)
        {
            //      printf ("CNLBLT %i\n", i);
            if (!channel[i]->selection && channel[i]->visible)
            {
                create_channel_surface(chs, (xcf_image_type)head.image_type, channel[i]->color, channel[i]->opacity);
                SDL_BlitSurface(chs, nullptr, surface, nullptr);
            }
            free_xcf_channel(channel[i]);
        }

        SDL_FreeSurface(chs);
#endif
    }

    return true;
}
