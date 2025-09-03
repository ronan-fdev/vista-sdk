using Vista.SDK;
using Vista.SDK.Transport.DataChannel;

/// <summary>
/// The calling to access a specific DataChannl is allowed by the following properties:
/// - ISO19848 LocalId
/// - Non-standardized name (as long as the DataChannel is configured)
/// - Index
/// As you can see. All properties are used similarly on the <see cref="DataChannelList"/> to access the <see cref="DataChannel"/>
/// </summary>
static class Transformer
{
    // Either read as file, or produce from database. Keep it in memory to improve performance
    public static readonly DataChannelListPackage DataChannelListPackage = JsonToDomain.ReadDataChannelList();

    public static LocalId Transform(string nonStandardTagName) => TransformToStandardizedName(nonStandardTagName);

    public static string Transform(LocalId nonStandardTagName) => TransformToNonStandardizedName(nonStandardTagName);

    public static string TransformToNonStandardizedName(LocalId standardTagName)
    {
        var dataChannel = DataChannelListPackage.DataChannelList[standardTagName];
        return dataChannel.DataChannelId.ShortId
            ?? throw new InvalidOperationException($"No ShortId found for LocalId {standardTagName}");
    }

    public static LocalId TransformToStandardizedName(string nonStandardTagName)
    {
        var dataChannel = DataChannelListPackage.DataChannelList[nonStandardTagName];
        return dataChannel.DataChannelId.LocalId;
    }

    // Of course, unecessary tranformation -  just to show that it is allowed
    public static string TransformToNonStandardizedName(string nonStandardTagName)
    {
        var dataChannel = DataChannelListPackage.DataChannelList[nonStandardTagName];
        return dataChannel.DataChannelId.ShortId
            ?? throw new InvalidOperationException($"No ShortId found for nonStandardTagName {nonStandardTagName}");
    }

    // Of course, unecessary tranformation -  just to show that it is allowed
    public static LocalId TransformToStandardizedName(LocalId localId)
    {
        var dataChannel = DataChannelListPackage.DataChannelList[localId];
        return dataChannel.DataChannelId.LocalId;
    }
}
