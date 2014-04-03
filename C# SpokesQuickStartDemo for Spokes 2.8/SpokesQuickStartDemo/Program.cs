using System;
using Plantronics.UC.SpokesWrapper;

namespace SpokesQuickStartDemo
{
    class Program
    {
        static Spokes spokes;

        static void Main(string[] args)
        {
            spokes = Spokes.Instance;
            spokes.PutOn += spokes_PutOn;
            spokes.TakenOff += spokes_TakenOff;
            spokes.Connect("Spokes Quick Start");

            Console.WriteLine("Press enter to quit...");
            Console.ReadLine();

            spokes.PutOn -= spokes_PutOn;
            spokes.TakenOff -= spokes_TakenOff;
            spokes.Disconnect();
        }

        static void spokes_TakenOff(object sender, WearingStateArgs e)
        {
            Console.WriteLine("Headset is not worn" + (e.m_isInitialStateEvent ? " (initial state)" : ""));
        }

        static void spokes_PutOn(object sender, WearingStateArgs e)
        {
            Console.WriteLine("Headset is worn"+(e.m_isInitialStateEvent?" (initial state)":""));
        }
    }
}
