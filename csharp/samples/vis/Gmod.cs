using System.Text;
using Vista.SDK;

static class UseGmod
{
    public static GmodNode GetNodeByCode(VisVersion visVersion, string code)
    {
        var gmod = VIS.Instance.GetGmod(visVersion);
        // Assumes it exists
        return gmod[code];
    }

    public static bool Traverse(VisVersion visVersion)
    {
        var gmod = VIS.Instance.GetGmod(visVersion);
        // Simplest form of traversal
        var completed = gmod.Traverse(
            (parents, node) =>
            {
                return TraversalHandlerResult.Continue;
            }
        );
        // Result is either true or false, based on whether the traversal completed or was stopped early
        return completed;
    }

    // Modify or create context based on your needs
    private sealed record YourCustomTraversalContext
    {
        public long Counter { get; set; }
        public GmodNode? TargetNode { get; set; }
    }

    public static bool TraverseWithState(VisVersion visVersion)
    {
        var gmod = VIS.Instance.GetGmod(visVersion);
        var context = new YourCustomTraversalContext();
        // Simplest form of traversal
        var completed = gmod.Traverse(
            context,
            (context, parents, node) =>
            {
                // Access context properties during the traversal
                context.Counter++;
                // Manually control the traversal
                if (context.Counter > 100)
                    return TraversalHandlerResult.Stop;
                return TraversalHandlerResult.Continue;
            }
        );
        // Result is either true or false, based on whether the traversal completed or was stopped early
        return completed;
    }

    public static bool TraverseFromNode(GmodNode startNode)
    {
        var gmod = VIS.Instance.GetGmod(startNode.VisVersion);
        // Without context
        var completed = gmod.Traverse(
            // Configure start node here
            startNode,
            (parents, node) =>
            {
                return TraversalHandlerResult.Continue;
            }
        );
        // With context
        var context = new YourCustomTraversalContext();
        completed = gmod.Traverse(
            context,
            // Configure start node here
            startNode,
            (context, parents, node) =>
            {
                context.Counter++;
                return TraversalHandlerResult.Continue;
            }
        );
        return completed;
    }

    public static GmodNode? FindFirstLeafNode(VisVersion visVersion)
    {
        var gmod = VIS.Instance.GetGmod(visVersion);
        var context = new YourCustomTraversalContext();
        // Utilize the context, and handle the case inside the traverse
        gmod.Traverse(
            context,
            (context, parents, node) =>
            {
                if (node.IsLeafNode)
                {
                    context.TargetNode = node;
                    return TraversalHandlerResult.Stop;
                }
                return TraversalHandlerResult.Continue;
            }
        );
        return context.TargetNode;
    }

    /// <summary>
    /// Notice that we dont use <see cref="Traverse" /> here, since we want to utilize the hierarchy of the graph
    /// </summary>
    public static string PrintTree(VisVersion visVersion, int maxDepth)
    {
        var sb = new StringBuilder();

        PrintNode(sb, VIS.Instance.GetGmod(visVersion).RootNode, maxDepth: maxDepth);
        return sb.ToString();
        // Essentially, this could be replaced with some custom handlers, such as HTML elements
        static void PrintNode(StringBuilder sb, GmodNode node, int level = 0, int maxDepth = int.MaxValue)
        {
            if (level > maxDepth)
                return;

            sb.AppendLine($"{new string(' ', level * 2)}- {node.Code}");
            foreach (
                var child in node.Children.OrderBy(
                    // Very bad comparer - just to display the use case - recommend to use natural sort
                    c => int.Parse(System.Text.RegularExpressions.Regex.Match(c.Code, @"\d+").Value)
                )
            )
            {
                PrintNode(sb, child, level + 1, maxDepth);
            }
        }
    }
}
