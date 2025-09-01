using Vista.SDK;

/// <summary>
/// Converts a GmodNode from one version to another.
/// Working with ISO19848, the <see cref="VIS.ConvertLocalId(LocalId, VisVersion)"/> is usually sufficient.
/// See <see cref="ConvertLocalId"/> for example usage.
/// </summary>
static class ConvertGmodNode
{
    public static GmodNode? Convert(string gmodNode, VisVersion sourceVersion, VisVersion targetVersion)
    {
        // Get Gmod for source version to get source GmodNode
        var gmod = VIS.Instance.GetGmod(sourceVersion);
        // Get source GmodNode
        var source = gmod[gmodNode];
        // Convert GmodNode to target VIS version
        var target = VIS.Instance.ConvertNode(source, targetVersion);
        return target;
    }
}
