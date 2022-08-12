using System;
using System.Collections.Generic;
using System.IO;

namespace SpaceFlight.Simulation
{
  public struct Vector2D
  {
    public double X { get; set; }
    public double Y { get; set; }

    public double Length
    {
      get { return Math.Sqrt(X * X + Y * Y); }
    }

    public static double Angle(Vector2D first, Vector2D second)
    {
      if (first.Length != 0 && second.Length != 0)
      {
        double angle = Math.Atan2(second.Y, second.X) - Math.Atan2(first.Y, first.X);
        return Math.IEEERemainder(angle, 2 * Math.PI);
      }
      else
      {
        return 0;
      }
    }

    public static Vector2D operator +(Vector2D first, Vector2D second)
    {
      return new Vector2D
      {
        X = first.X + second.X,
        Y = first.Y + second.Y,
      };
    }

    public static Vector2D operator -(Vector2D first, Vector2D second)
    {
      return new Vector2D
      {
        X = first.X - second.X,
        Y = first.Y - second.Y,
      };
    }

    public static Vector2D operator *(double scalar, Vector2D vector)
    {
      return new Vector2D
      {
        X = scalar * vector.X,
        Y = scalar * vector.Y,
      };
    }
  }

  public class Planet
  {
    public Vector2D Position { get; set; }
    public double Radius { get; set; }
  }

  public class Ship
  {
    public Vector2D Position { get; set; }
    public Vector2D Velocity { get; set; }
    public double Angle { get; set; }
    public double Turn { get; set; }

    public Vector2D Direction
    {
      get { return new Vector2D { X = Math.Cos(Angle), Y = Math.Sin(Angle) }; }
    }
  }

  public interface IShipController
  {
    void UpdateShip(TestCase testCase, Vector2D gravity);
  }

  public class TestCase
  {
    public IList<Planet> Planets { get; private set; }
    public IList<Vector2D> Waypoints { get; private set; }
    public Vector2D StartingPosition { get; set; }

    public Ship Ship { get; set; }
    public int TimeSteps { get; set; }
    public int TargetWaypoint { get; set; }

    public bool IsActive { get; set; }

    public bool TraceMovement { get; set; }
    public IList<Vector2D> Trace { get; private set; }

    public TestCase()
    {
      Planets = new List<Planet>();
      Waypoints = new List<Vector2D>();
      Trace = new List<Vector2D>();
    }

    public void Reset()
    {
      Ship = new Ship
      {
        Position = StartingPosition,
        Velocity = new Vector2D(),
        Angle = Vector2D.Angle(new Vector2D { X = 1 }, Waypoints[0] - StartingPosition)
      };
      TimeSteps = 0;
      TargetWaypoint = 0;
      IsActive = true;
      Trace.Clear();
    }

    public void Simulate(IShipController controller, int steps)
    {
      for (int i = 0; i < steps && IsActive; ++i)
      {
        Simulate(controller);
      }
    }

    private void Simulate(IShipController controller)
    {
      Vector2D gravity = new Vector2D();
      foreach (Planet planet in Planets)
      {
        Vector2D distance = planet.Position - Ship.Position;
        if (distance.Length <= planet.Radius)
        {
          IsActive = false;
        }
        gravity += 15 * 0.001 * (Math.Pow(planet.Radius, 3) / Math.Pow(distance.Length, 3)) * distance;
      }
      Ship.Velocity += gravity;

      controller.UpdateShip(this, gravity);
      ++TimeSteps;

      const double MaxTurn = 2 * Math.PI / 25;
      Ship.Angle += Math.Max(-MaxTurn, Math.Min(MaxTurn, Ship.Turn));

      Ship.Velocity += 12 * 0.02 * Ship.Direction;
      Ship.Position += Ship.Velocity;

      double targetDistance = (Ship.Position - Waypoints[TargetWaypoint]).Length;
      if (targetDistance < 20)
      {
        ++TargetWaypoint;
        if (TargetWaypoint == Waypoints.Count)
        {
          IsActive = false;
        }
      }
      else if (targetDistance > 1500)
      {
        IsActive = false;
      }

      if (TraceMovement)
      {
        Trace.Add(Ship.Position);
      }
    }

    public static TestCase Load(string filename)
    {
      TestCase testCase = new TestCase();
      using (var stream = new StreamReader(filename))
      {
        string line;
        while ((line = stream.ReadLine()).Length != 0)
        {
          string[] parts = line.Split(' ');
          if (parts.Length != 3)
          {
            throw new Exception();
          }
          testCase.Planets.Add(new Planet
          {
            Position = new Vector2D { X = double.Parse(parts[0]), Y = double.Parse(parts[1]) },
            Radius = double.Parse(parts[2])
          });
        }

        while ((line = stream.ReadLine()).Length != 0)
        {
          string[] parts = line.Split(' ');
          if (parts.Length != 2)
          {
            throw new Exception();
          }
          testCase.Waypoints.Add(new Vector2D { X = double.Parse(parts[0]), Y = double.Parse(parts[1]) });
        }

        {
          line = stream.ReadLine();
          string[] parts = line.Split(' ');
          if (parts.Length != 2)
          {
            throw new Exception();
          }
          testCase.StartingPosition = new Vector2D { X = double.Parse(parts[0]), Y = double.Parse(parts[1]) };
        }

        if (stream.ReadToEnd().Trim().Length != 0)
        {
          throw new Exception();
        }
      }

      return testCase;
    }

    public static void Save(TestCase testCase, string filename)
    {
      using (var stream = new StreamWriter(filename))
      {
        foreach (Planet planet in testCase.Planets)
        {
          stream.WriteLine("{0:0} {1:0} {2:0}", planet.Position.X, planet.Position.Y, planet.Radius);
        }
        stream.WriteLine();

        foreach (Vector2D waypoint in testCase.Waypoints)
        {
          stream.WriteLine("{0:0} {1:0}", waypoint.X, waypoint.Y);
        }
        stream.WriteLine();

        stream.WriteLine("{0:0} {1:0}", testCase.StartingPosition.X, testCase.StartingPosition.Y);
      }
    }

    public static TestCase CreateRandom(int numPlanets, int numWaypoints)
    {
      Random random = new Random();
      TestCase testCase = new TestCase();

      testCase.StartingPosition = new Vector2D
      {
        X = 200 + random.NextDouble() * 600,
        Y = 200 + random.NextDouble() * 600
      };

      int tries = 0;
      while (testCase.Planets.Count != numPlanets && tries < numPlanets * 100)
      {
        Planet planet = new Planet
        {
          Position = new Vector2D
          {
            X = 100 + random.NextDouble() * 800,
            Y = 100 + random.NextDouble() * 800
          },
          Radius = 15 + random.NextDouble() * 15
        };
        ++tries;

        if (!IsTooClose(planet.Position, testCase, planet.Radius + 200))
        {
          testCase.Planets.Add(planet);
        }
      }

      tries = 0;
      while (testCase.Waypoints.Count != numWaypoints && tries < numWaypoints * 100)
      {
        Vector2D waypoint = new Vector2D
        {
          X = 100 + random.NextDouble() * 800,
          Y = 100 + random.NextDouble() * 800
        };
        ++tries;

        if (!IsTooClose(waypoint, testCase, 50))
        {
          testCase.Waypoints.Add(waypoint);
        }
      }

      return testCase;
    }

    private static bool IsTooClose(Vector2D position, TestCase testCase, double minDistance)
    {
      if ((position - testCase.StartingPosition).Length < minDistance)
      {
        return true;
      }

      foreach (Planet planet in testCase.Planets)
      {
        if ((position - planet.Position).Length - planet.Radius < minDistance)
        {
          return true;
        }
      }

      foreach (Vector2D waypoint in testCase.Waypoints)
      {
        if ((position - waypoint).Length < minDistance)
        {
          return true;
        }
      }

      return false;
    }
  }
}
