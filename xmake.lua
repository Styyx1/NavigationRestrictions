-- set minimum xmake version
set_xmakever("2.8.2")

-- includes
includes("lib/commonlibsse-ng")

-- set project
set_project("NavigationRestrictions")
set_version("2.0.0")
set_license("GPL-3.0")

-- set defaults
set_languages("c++23")
set_warnings("allextra")


-- set policies
set_policy("package.requires_lock", true)

-- add rules
add_rules("mode.debug", "mode.releasedbg")
add_rules("plugin.vsxmake.autoupdate")
set_defaultmode("releasedbg")
set_config("skse_xbyak", true)

-- packages
add_requires("simpleini", "xbyak")
add_requires("spdlog", { configs = { header_only = false } })
-- add_requires("glaze", {configs = { header_only = true}})

-- targets
target("NavigationRestrictions")
    -- add dependencies to target
    add_deps("commonlibsse-ng")
    add_packages("fmt", "spdlog", "simpleini", "xbyak")

    -- add commonlibsse-ng plugin
    add_rules("commonlibsse-ng.plugin", {
        name = "NavigationRestrictions",
        author = "styyx",
        description = "Restricts the use of your map and compass to having the actual items in your inventory"
    })

    -- add src files
    add_files("src/**.cpp")
    add_headerfiles("src/**.h")
    add_includedirs("src")
    set_pcxxheader("src/pch.h")
