using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace SpaceFlight.Simulation
{
  public class Evaluator
  {
    private List<TestCase> testCases;

    public Evaluator(string path)
    {
      testCases = new List<TestCase>();
      foreach (string filename in Directory.EnumerateFiles(path))
      {
        try
        {
          testCases.Add(TestCase.Load(filename));
        }
        catch
        {
          throw new Exception("Invalid test case file: " + filename);
        }
      }

      if (testCases.Count == 0)
      {
        throw new Exception("No test cases loaded!");
      }
    }
    
    public double Evaluate(AiShipController controller)
    {
      const int MaxSteps = 2500;

      var fitnesses = new List<double>();
      foreach (TestCase testCase in testCases)
      {
        testCase.Reset();
        testCase.Simulate(controller, MaxSteps);

        double fitness = (double)testCase.TargetWaypoint / testCase.Waypoints.Count;
        if (testCase.TargetWaypoint == testCase.Waypoints.Count)
        {
          fitness += 1 - (double)testCase.TimeSteps / MaxSteps;
        }
        fitnesses.Add(fitness);
      }

      double minFitness = fitnesses.Min();
      return fitnesses.Average(f => Math.Min(f, minFitness + 250));
    }
  }
}
