using System;
using System.Collections.Generic;
using System.Linq;

namespace SpaceFlight.Simulation
{
  public class AiShipController : IShipController
  {
    private INode root;

    public AiShipController(string nodes)
    {
      root = Parse(nodes.Trim().Split(' ').AsEnumerable().GetEnumerator());
    }

    public void UpdateShip(TestCase testCase, Vector2D gravity)
    {
      var context = new Dictionary<string, double>();
      context["pi"] = Math.PI;

      Vector2D target = testCase.Waypoints[testCase.TargetWaypoint] - testCase.Ship.Position;
      context["td"] = target.Length;
      context["ta"] = Vector2D.Angle(testCase.Ship.Direction, target);

      context["vm"] = testCase.Ship.Velocity.Length;
      context["va"] = Vector2D.Angle(testCase.Ship.Direction, testCase.Ship.Velocity);
      context["gm"] = gravity.Length;
      context["ga"] = Vector2D.Angle(testCase.Ship.Direction, gravity);

      var nearest = testCase.Planets
        .DefaultIfEmpty(new Planet { Position = new Vector2D { X = -2000, Y = -2000 } })
        .Select(p => new { Distance = p.Position - testCase.Ship.Position, Raidus = p.Radius })
        .OrderBy(p => p.Distance.Length - p.Raidus)
        .First();

      context["npd"] = nearest.Distance.Length - nearest.Raidus;
      context["npa"] = Vector2D.Angle(testCase.Ship.Direction, nearest.Distance);

      testCase.Ship.Turn = root.Evaluate(context);
    }

    private INode Parse(IEnumerator<string> tokens)
    {
      tokens.MoveNext();
      string symbol = tokens.Current;
      if (new [] { "+", "-", "*", "/" }.Contains(symbol))
      {
        return new OperatorNode(symbol[0], Parse(tokens), Parse(tokens));
      }
      else if (symbol == "iflt")
      {
        return new BranchNode(Parse(tokens), Parse(tokens), Parse(tokens), Parse(tokens));
      }
      else if (new [] { "td", "ta", "vm", "va", "npd", "npa" }.Contains(symbol))
      {
        return new VariableNode(symbol);
      }
      else
      {
        if (symbol.StartsWith("D_") || symbol.StartsWith("I_"))
        {
          symbol = symbol.Substring(2);
        }
        return new ValueNode(double.Parse(symbol));
      }
    }
  }

  internal interface INode
  {
    double Evaluate(IDictionary<string, double> context);
  }

  internal class OperatorNode : INode
  {
    private char oper;
    private INode firstArg;
    private INode secondArg;

    public OperatorNode(char oper, INode firstArg, INode secondArg)
    {
      this.oper = oper;
      this.firstArg = firstArg;
      this.secondArg = secondArg;
    }

    public double Evaluate(IDictionary<string, double> context)
    {
      double first = firstArg.Evaluate(context);
      double second = secondArg.Evaluate(context);
      switch (oper)
      {
        case '+':
        {
          return first + second;
        }
        case '-':
        {
          return first - second;
        }
        case '*':
        {
          return first * second;
        }
        case '/':
        {
          return Math.Abs(second) > 1e-5 ? first / second : 1;
        }
        default:
        {
          throw new ArgumentException();
        }
      }
    }
  }

  internal class BranchNode : INode
  {
    private INode first, second;
    private INode less, greater;

    public BranchNode(INode first, INode second, INode lessEqual, INode greater)
    {
      this.first = first;
      this.second = second;
      this.less = lessEqual;
      this.greater = greater;
    }

    public double Evaluate(IDictionary<string, double> context)
    {
      if (first.Evaluate(context) < second.Evaluate(context))
      {
        return less.Evaluate(context);
      }
      else
      {
        return greater.Evaluate(context);
      }
    }
  }

  internal class ValueNode : INode
  {
    private double value;

    public ValueNode(double value)
    {
      this.value = value;
    }

    public double Evaluate(IDictionary<string, double> context)
    {
      return value;
    }
  }

  internal class VariableNode : INode
  {
    private string variable;

    public VariableNode(string variable)
    {
      this.variable = variable;
    }

    public double Evaluate(IDictionary<string, double> context)
    {
      return context[variable];
    }
  }
}
