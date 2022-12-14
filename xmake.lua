if is_mode("debug") then
    add_rules("mode.debug")
elseif is_mode("release") then
    add_rules("mode.release")
end

target("zinc")
    set_kind("static")
    add_includedirs("include", {public = true})
    if is_kind("shared") then
        add_defines("ZINC_CONFIG_SHARED_LIB" , "ZINC_EXPORTS")
    end
    set_languages("cxx17")
    add_files("src/**.cpp")

    if is_plat("windows") then
        add_syslinks("User32", "Shell32", "Gdi32", "Kernel32")
    end

target("zinctest")
    set_kind("binary")
    add_files("tests/zincpp.cpp")
    set_languages("cxx17")
    if is_kind("shared") then
        add_defines("ZINC_CONFIG_SHARED_LIB")
    end
    add_deps("zinc")
