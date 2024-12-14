using System;
using System.IO;
using UnrealBuildTool;

public class libandroid : ModuleRules
{
    public libandroid(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            string libandroid = Path.Combine(ModuleDirectory, "lib", "Android", "arm64-v8a", "liblibandroid.so");
            PublicAdditionalLibraries.Add(libandroid);
            PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

            string libopencv = Path.Combine(ModuleDirectory, "lib", "Android", "arm64-v8a", "libopencv_java4.so");
            PublicAdditionalLibraries.Add(libopencv);

            string umediapipe = Path.Combine(ModuleDirectory, "lib", "Android", "arm64-v8a", "libUMediapipe.so");
            PublicAdditionalLibraries.Add(umediapipe);

            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "libandroid_APL.xml"));
        }
    }
}