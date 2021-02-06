local to_save = {
    abc = "def",
    xyz = 3,
    foo = {
        {
            p = "1",
        },
        {
            1, 2, 3
        },
        {
            a = {
                a = {
                    a = {
                        a = "b"
                    }
                }
            }
        }
    }
}

savesystem.save(to_save)
print("")
print(savesystem.get_value_from_saved{"abc"})
print(savesystem.get_saved().foo[3].a.a.a.a)
print("")