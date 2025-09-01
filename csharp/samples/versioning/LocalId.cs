using Vista.SDK;

/// <summary>
/// Converts a LocalId from one version to another.
/// </summary>
static class ConvertLocalId
{
    public static LocalId? Convert(string localId, VisVersion targetVersion)
    {
        // Parse input path into domain model
        var source = LocalId.Parse(localId);
        // Convert LocalId to target VIS version
        var target = VIS.Instance.ConvertLocalId(source, targetVersion);
        return target;
    }
}
