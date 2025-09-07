using Vista.SDK;

/// <summary>
/// Converts a GmodPath from one version to another.
/// Working with ISO19848, the <see cref="VIS.ConvertLocalId(LocalId, VisVersion)"/> is usually sufficient.
/// See <see cref="ConvertLocalId"/> for example usage.
/// </summary>
static class ConvertGmodPath
{
    public static GmodPath? Convert(string gmodPath, VisVersion sourceVersion, VisVersion targetVersion)
    {
        // Parse input path into domain model
        var source = GmodPath.Parse(gmodPath, sourceVersion);
        // Convert GmodPath to target VIS version
        var target = VIS.Instance.ConvertPath(source, targetVersion);
        return target;
    }
}
