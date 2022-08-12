using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using SpaceFlight.Simulation;

namespace SpaceFlight.GUI
{
  public class SimulationView : FrameworkElement
  {
    public TestCase TestCase { get; set; }

    private ImageSource planetImage;
    private ImageSource shipImage;
    private ImageSource explosionImage;

    public SimulationView()
    {
      planetImage = new BitmapImage(new Uri("pack://application:,,,/Images/Planet.png"));
      shipImage = new BitmapImage(new Uri("pack://application:,,,/Images/Ship.png"));
      explosionImage = new BitmapImage(new Uri("pack://application:,,,/Images/Explosion.png"));
    }

    protected override void OnRender(DrawingContext context)
    {
      base.OnRender(context);
      if (TestCase != null)
      {
        context.PushClip(new RectangleGeometry(new Rect(0, 0, ActualWidth, ActualHeight)));
        context.PushTransform(GetTransform());

        foreach (Vector2D point in TestCase.Trace)
        {
          context.DrawEllipse(Brushes.Gray, null, new Point(point.X, point.Y), 2, 2);
        }

        foreach (Planet planet in TestCase.Planets)
        {
          context.DrawImage(planetImage, new Rect(planet.Position.X - planet.Radius,
            planet.Position.Y - planet.Radius, planet.Radius * 2, planet.Radius * 2));
        }

        for (int i = 0; i < TestCase.Waypoints.Count; ++i)
        {
          Brush brush;
          if (i < TestCase.TargetWaypoint)
          {
            brush = Brushes.Gray;
          }
          else if (i == TestCase.TargetWaypoint)
          {
            brush = Brushes.Red;
          }
          else
          {
            brush = Brushes.Black;
          }
          Vector2D waypoint = TestCase.Waypoints[i];
          context.DrawEllipse(null, new Pen(brush, 3), new Point(waypoint.X, waypoint.Y), 10, 10);
          context.DrawEllipse(brush, null, new Point(waypoint.X, waypoint.Y), 3, 3);
        }

        if (TestCase.IsActive || TestCase.TargetWaypoint == TestCase.Waypoints.Count)
        {
          Vector2D position = TestCase.Ship.Position;
          context.PushTransform(new ScaleTransform(0.6, 0.6, position.X, position.Y));
          context.PushTransform(new RotateTransform(TestCase.Ship.Angle / Math.PI * 180, position.X, position.Y));
          context.DrawImage(shipImage, new Rect(
            position.X - shipImage.Width / 2, position.Y - shipImage.Height / 2,
            shipImage.Width, shipImage.Height));
        }
        else
        {
          context.DrawImage(explosionImage, new Rect(
            TestCase.Ship.Position.X - explosionImage.Width / 2, TestCase.Ship.Position.Y - explosionImage.Height / 2,
            explosionImage.Width, explosionImage.Height));
        }
      }
    }

    public Transform GetTransform()
    {
      TransformGroup transform = new TransformGroup();
      double size = Math.Min(ActualWidth, ActualHeight);
      transform.Children.Add(new ScaleTransform(size / 1000, size / 1000));
      if (ActualWidth > ActualHeight)
      {
        transform.Children.Add(new TranslateTransform((ActualWidth - size) / 2, 0));
      }
      else
      {
        transform.Children.Add(new TranslateTransform(0, (ActualHeight - size) / 2));
      }
      return transform;
    }
  }
}
