using System;
using System.IO;
using System.Windows;
using System.Windows.Threading;
using Microsoft.Win32;
using SpaceFlight.Simulation;

using System.Linq;

namespace SpaceFlight.GUI
{
  public partial class MainWindow : Window
  {
    private DispatcherTimer timer;
    private TestCase testCase;
    private IShipController aiController;
    private IShipController controller;
    private DateTime simulationStart;

    public MainWindow()
    {
      InitializeComponent();

      timer = new DispatcherTimer();
      timer.Interval = TimeSpan.FromMilliseconds(40);
      timer.Tick += Timer_Tick;
    }

    void Timer_Tick(object sender, EventArgs e)
    {
      int targetSteps = (int)((DateTime.Now - simulationStart).TotalMilliseconds / 40);
      testCase.Simulate(controller, targetSteps - testCase.TimeSteps);
      if (!testCase.IsActive)
      {
        StopSimulation();
      }
      Time.Text = String.Format("Time: {0} s", testCase.TimeSteps / 25.0);
      Waypoints.Text = String.Format("Waypoints: {0} / {1}", testCase.TargetWaypoint, testCase.Waypoints.Count);
      View.InvalidateVisual();
    }

    private void LoadButton_Click(object sender, RoutedEventArgs e)
    {
      var dialog = new OpenFileDialog();
      dialog.Filter = "Test case files|*.txt";
      dialog.InitialDirectory = Directory.GetCurrentDirectory();
      if (dialog.ShowDialog() == true)
      {
        try
        {
          SetTestCase(TestCase.Load(dialog.FileName));
          SaveButton.IsEnabled = true;
          StartButton.IsEnabled = controller != null;
        }
        catch (Exception)
        {
          MessageBox.Show("Error loading test case.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
      }
    }

    private void RandomButton_Click(object sender, RoutedEventArgs e)
    {
      int numPlanets, numWaypoints;
      if (!int.TryParse(RandomPlanets.Text, out numPlanets) || numPlanets < 0)
      {
        MessageBox.Show("Invalid number of planets.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        return;
      }
      if (!int.TryParse(RandomWaypoints.Text, out numWaypoints) || numWaypoints < 1)
      {
        MessageBox.Show("Invalid number of waypoints.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        return;
      }
      SetTestCase(TestCase.CreateRandom(numPlanets, numWaypoints));
      SaveButton.IsEnabled = true;
      StartButton.IsEnabled = controller != null;
    }

    private void SetTestCase(TestCase testCase)
    {
      testCase.Reset();
      testCase.TraceMovement = TraceMovement.IsChecked == true;
      this.testCase = testCase;
      View.TestCase = testCase;
      View.InvalidateVisual();
      Time.Text = string.Empty;
      Waypoints.Text = string.Empty;
    }

    private void SaveButton_Click(object sender, RoutedEventArgs e)
    {
      var dialog = new SaveFileDialog();
      dialog.InitialDirectory = Directory.GetCurrentDirectory();
      dialog.Filter = "Test case files|*.txt";
      if (dialog.ShowDialog() == true)
      {
        try
        {
          TestCase.Save(testCase, dialog.FileName);
        }
        catch (Exception)
        {
          MessageBox.Show("Error saving test case.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
        }
      }
    }

    private void ManualFlight_Click(object sender, RoutedEventArgs e)
    {
      if (ManualFlight.IsChecked != true)
      {
        LoadAI.IsEnabled = true;
        controller = aiController;
      }
      else
      {
        LoadAI.IsEnabled = false;
        controller = new MouseShipController(View);
      }
      StartButton.IsEnabled = testCase != null && controller != null;
    }

    private void LoadAI_Click(object sender, RoutedEventArgs e)
    {
      var dialog = new OpenFileDialog();
      dialog.InitialDirectory = Directory.GetCurrentDirectory();
      if (dialog.ShowDialog() == true)
      {
        using (var stream = new StreamReader(dialog.FileName))
        {
          try
          {
            aiController = new AiShipController(stream.ReadToEnd());
            controller = aiController;
          }
          catch
          {
            MessageBox.Show("Error loading AI.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
          }
        }
      }
      StartButton.IsEnabled = testCase != null;
    }

    private void TraceMovement_Click(object sender, RoutedEventArgs e)
    {
      if (testCase != null)
      {
        testCase.TraceMovement = TraceMovement.IsChecked == true;
      }
    }

    private void StartButton_Click(object sender, RoutedEventArgs e)
    {
      LoadButton.IsEnabled = false;
      RandomButton.IsEnabled = false;
      SaveButton.IsEnabled = false;
      ManualFlight.IsEnabled = false;
      LoadAI.IsEnabled = false;
      TraceMovement.IsEnabled = false;

      StartButton.Visibility = Visibility.Collapsed;
      StopButton.Visibility = Visibility.Visible;

      testCase.Reset();
      simulationStart = DateTime.Now;
      timer.Start();
    }

    private void StopButton_Click(object sender, RoutedEventArgs e)
    {
      StopSimulation();
    }

    private void StopSimulation()
    {
      timer.Stop();

      LoadButton.IsEnabled = true;
      RandomButton.IsEnabled = true;
      SaveButton.IsEnabled = true;
      ManualFlight.IsEnabled = true;
      LoadAI.IsEnabled = ManualFlight.IsChecked != true;
      TraceMovement.IsEnabled = true;

      StopButton.Visibility = Visibility.Collapsed;
      StartButton.Visibility = Visibility.Visible;
    }
  }
}
