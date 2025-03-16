using System;
using System.IO;
using UnrealBuildTool;

public class libHandyVR : ModuleRules
{
    public libHandyVR(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PublicAdditionalLibraries.Add(Path.Combine(PluginDirectory, "Binaries", "Win64", "UMediapipe.lib"));

            RuntimeDependencies.Add("$(TargetOutputDir)/opencv_world3410.dll", Path.Combine(PluginDirectory, "Binaries", "Win64", "opencv_world3410.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/UMediapipe.dll", Path.Combine(PluginDirectory, "Binaries", "Win64", "UMediapipe.dll"));
        }
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Android", "arm64-v8a", "libopencv_java4.so"));
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Android", "arm64-v8a", "libUMediapipe.so"));

            string PluginPath = Utils.MakePathRelativeTo(ModuleDirectory, Target.RelativeEnginePath);
            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(PluginPath, "libHandyVR_APL.xml"));
        }
    }
}