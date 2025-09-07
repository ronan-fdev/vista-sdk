using Vista.SDK;

Console.WriteLine($"=========== VIS sample ===========");

var visVersion = VisVersion.v3_9a;

// Get node by code
var node = UseGmod.GetNodeByCode(visVersion, "411.1");
Console.WriteLine($"GetNodeByCode: {node}");

// Basic traversal
var result = UseGmod.Traverse(visVersion);
Console.WriteLine($"Traverse completed: {result} Expect='True'");

// Traverse from node
result = UseGmod.TraverseFromNode(node);
Console.WriteLine($"TraverseFromNode completed: {result} Expect='True'");

// Traverse with state
result = UseGmod.TraverseWithState(visVersion);
Console.WriteLine($"TraverseWithState completed: {result} Expect='False'");

node = UseGmod.FindFirstLeafNode(visVersion);
Console.WriteLine($"FindFirstLeafNode: {node}");

var treeString = UseGmod.PrintTree(visVersion, maxDepth: 1);
Console.WriteLine($"PrintTree:\n{treeString}");
