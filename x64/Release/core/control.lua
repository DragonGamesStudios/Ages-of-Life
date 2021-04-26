print("\nctrl\n")

interface = {
    test = function(x)
        print("Parameter", x)
    end
}

remote.add_interface("core-inter", interface)

script.on_load(function()
    print("The game is loaded")
end)

script.on_configuration_changed(function(mod_list, this_added)
    for k, v in pairs(mod_list) do
        print(k, v)
    end
end)

script.on_ready(function()
    print("The game is ready")
end)

script.on_save(function()
    print("The game is being saved")
end)

script.on_event(defines.events.on_tick, function(event)
    if event.tick % 60 == 0 then
        print(event.tick)
    end
end)