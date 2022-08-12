using System;
using System.Windows.Input;
using SpaceFlight.Simulation;

namespace SpaceFlight.GUI
{
  public class MouseShipController : IShipController
  {
    SimulationView view;

    public MouseShipController(SimulationView view)
    {
      this.view = view;
    }

    public void UpdateShip(TestCase testCase, Vector2D gravity)
    {
      if (Mouse.PrimaryDevice.LeftButton == MouseButtonState.Pressed)
      {
        var mousePoint = view.GetTransform().Inverse.Transform(Mouse.PrimaryDevice.GetPosition(view));
        var mouseVector = new Vector2D { X = mousePoint.X, Y = mousePoint.Y };
        var difference = mouseVector - testCase.Ship.Position;
        testCase.Ship.Turn = Vector2D.Angle(testCase.Ship.Direction, difference);
      }
      else
      {
        testCase.Ship.Turn = 0;
      }
    }
  }
}
