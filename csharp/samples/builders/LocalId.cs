using Vista.SDK;

/// <summary>
/// Examples of how to use the LocalId builder
/// </summary>
static class BuildLocalId
{
    /// <summary>
    /// Example of a LocalId builder usage
    /// </summary>
    public static LocalId BuildNew(
        VisVersion visVersion,
        string primaryItem,
        string quantity
    // Dropping additional metadata tags, as the jist is the same
    )
    {
        // Get codebooks for metadata creation
        var codebooks = VIS.Instance.GetCodebooks(visVersion);
        // Init from scratch
        var builder = LocalIdBuilder
            .Create(visVersion)
            // Allows for chaining
            .WithPrimaryItem(GmodPath.Parse(primaryItem, visVersion));
        // Or no chaining
        builder = builder.WithMetadataTag(codebooks.CreateTag(CodebookName.Quantity, quantity));
        return builder.Build();
    }

    public static LocalId BuildFromExisting(string inputLocalId)
    {
        // Alt 1 - Init builder directly
        var builder = LocalIdBuilder.Parse(inputLocalId);
        // Alt 2- Or continue from existing LocalId
        var localId = LocalId.Parse(inputLocalId);
        builder = localId.Builder;
        // Modify as needed
        return builder.Build();
    }

    /// <summary>
    /// Enables verbose mode. See <see cref="LocalId.VerboseMode"/> and <see cref="LocalId.ToString"/>
    /// </summary>
    /// <returns></returns>
    public static LocalId BuildVerbose(LocalId localId)
    {
        // Access builder from existing LocalId
        var builder = localId.Builder;
        // Enables verbose mode
        return builder.WithVerboseMode(true).Build();
    }

    public static LocalIdBuilder? RemoveProperties(LocalId localId)
    {
        // Access builder from existing LocalId
        var builder = localId.Builder;
        // Remove properties as needed
        builder = builder.WithoutPrimaryItem();
        builder = builder.WithoutSecondaryItem();
        builder = builder.WithoutMetadataTag(CodebookName.Quantity);
        builder = builder.WithoutVisVersion();
        if (builder.IsValid == false)
        {
            // Note - builder is now invalid, as a LocalId must have a VIS version, primary item and at least one metadata tag
            // However, it is still possible to access properties and ToString
            return null;
        }
        return builder;
    }
}
