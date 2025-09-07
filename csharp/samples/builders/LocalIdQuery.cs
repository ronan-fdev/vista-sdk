using Vista.SDK;

/// <summary>
/// Examples of how to use the LocalIdQuery builder.
/// The LocalIdQuery builder allows for constructing queries against LocalId instances.
/// Due to the wide range of configurations of a LocalId, this example only covers the most common use cases
/// </summary>
static class BuildLocalIdQuery
{
    public static bool MatchSelf(LocalId localId)
    {
        // Init builder from LocalId - this will only match this specific LocalId
        var builder = LocalIdQueryBuilder.From(localId);
        // Modify as needed
        return builder.Build().Match(localId);
    }

    public static bool MatchAll(LocalId localId)
    {
        // Simply create an empty builder
        var builder = LocalIdQueryBuilder.Empty();
        return builder.Build().Match(localId);
    }

    public static bool MatchAcrossVisVersions(LocalId oldLocalId, LocalId newLocalId)
    {
        var builder = LocalIdQueryBuilder.From(oldLocalId);
        // No action required - Automatically converts the localids up to latest VIS version
        return builder.Build().Match(newLocalId);
    }

    public static bool MatchMetadataTag(CodebookName tagName, string tagValue, LocalId matchLocalId)
    {
        var builder = LocalIdQueryBuilder.Empty();
        builder = builder.WithTags(t => t.WithTag(tagName, tagValue).Build());
        return builder.Build().Match(matchLocalId);
    }

    public static bool MatchAllInstancesOfAGmodPath(LocalId initLocalId, LocalId matchLocalId)
    {
        var builder = LocalIdQueryBuilder.From(initLocalId).WithoutLocations();

        return builder.Build().Match(matchLocalId);
    }

    public static bool MatchAllInstancesOfAGmodNodeInGmodPath(LocalId initLocalId, string node, LocalId matchLocalId)
    {
        var builder = LocalIdQueryBuilder.From(initLocalId);
        // Configure the specified node
        builder = builder.WithPrimaryItem(p => p.WithNode(n => n[node], matchAllLocations: true).Build());
        return builder.Build().Match(matchLocalId);
    }
}
