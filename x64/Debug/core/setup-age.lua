aol_register({
    {
        type = "age",
        name = "vanilla-age-stone-age",
        neighbor_age = "vanilla-age-copper-age",
        icon_path = "__base__/graphics/age/icons/stone-age.png",
        master_image_path = "__base__/graphics/age/labels/stone-age.png",
        font_color = {r=51, g=51, b=51},
        master_color = {r=100, g=100, b=100},
        neighbor_specifier = "after"
    },
    {
        type = "age",
        name = "vanilla-age-copper-age",
        neighbor_age = "vanilla-age-stone-age",
        icon_path = "__base__/graphics/age/icons/copper-age.png",
        master_image_path = "__base__/graphics/age/labels/stone-age.png",
        font_color = {r=250, g=174, b=74},
        master_color = {r=100, g=100, b=100},
        neighbor_specifier = "previous"
    }
})