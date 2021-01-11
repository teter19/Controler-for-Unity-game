using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO.Ports;
using System;
using System.Globalization;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
public class abc1 : MonoBehaviour
{

    //SerialPort stream = new SerialPort("COM5", 9600, Parity.None, 8, StopBits.One);
    //SerialPort stream = new SerialPort("COM5", 9600);
    public string theName;
    public GameObject inputField;
    public GameObject textDisplay;
    public SerialPort stream;

    public string receivedstring;
    //polozenie kolki xyz;
    public string x;
    public string y;
    public string z;
    public string xyz;  //string do ktorego wpisywany jest CALY string z UART
    char[] mychar = { ',' };    //do segregowania danych z UART
    public Rigidbody rb;

    public Color myColor;
    public string colorrr;    //kolor kolki odczytany z UART
    //public Renderer myRenderer;
    public Renderer rend;
    public float aFloat;    //wartosc przeswitu kolor / obiektu (alpha)

    private float time = 0.0f;
    private bool isMoving = false;
    private bool isJumpPressed = false;

    //dla Bullet
    public float moveForce = 0f;
    public GameObject bullet;
    public Transform gun;
    public float shootRate = 0f;
    public float shootForce = 0f;
    private float shootRateTimeStamp = 0f;

    public float restart;
    Vector3 m_YAxis;
    void Start()
    {
        StoreName();
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        Read(); //czytaj dane z portu

        string[] xyz = receivedstring.Split(mychar);
        x = xyz[2]; //potencjometr nr 1, (lewo / prawo), pin A8
        float xxx=float.Parse(x, CultureInfo.InvariantCulture.NumberFormat);
        float xxxx = xxx - 1.75f;
        y = xyz[3]; //potencjometr nr 2, (gora / dol), pin A9
        float yyy = float.Parse(y, CultureInfo.InvariantCulture.NumberFormat);
        float yyyy = yyy - 1.75f;

        isJumpPressed = Input.GetButtonDown("Jump");
        rb.velocity = new Vector3(xxxx*8, -3, yyyy*8);
        isMoving = true;
        Debug.Log("jump");  //spr czy dziala, wysyla "jump" do konsoli

        //TSI Slider, zmiana koloru kolki
        colorrr = xyz[1];
        float colorr = float.Parse(colorrr, CultureInfo.InvariantCulture.NumberFormat);
        // myColor = new Color(1f, 1f, 1f, 1f);

        //SPR wartosc z TSI Slider i ustawm kolor kulki
        if ((colorr < 100) && (colorr > 80))
        {
            rend.material.SetColor("_Color", Color.red);
        }
        else if ((colorr < 80) && (colorr > 60))
        {
            rend.material.SetColor("_Color", Color.green);
        }

        else if ((colorr < 60) && (colorr > 40))
        {
            rend.material.SetColor("_Color", Color.blue);
        }
        else if ((colorr < 40) && (colorr > 20))
        {
            rend.material.SetColor("_Color", Color.yellow);
        }
        else if ((colorr < 20) && (colorr > 0))
        {
            rend.material.SetColor("_Color", Color.white);
        }

        //SPR czy zresetowac
        //xyz[4] to jest dana z przyciskow
        if (xyz[4]=="1")
        {
            SceneManager.LoadScene(SceneManager.GetActiveScene().name);
        }
        if (xyz[4] == "2")
        {
            if(Time.time > shootRateTimeStamp)
            {
                GameObject go = (GameObject)Instantiate(bullet,gun.position,gun.rotation);
                go.GetComponent<Rigidbody>().AddForce(gun.forward * shootForce);
                shootRateTimeStamp = Time.time + shootRate;

            }
        }
        //zamyka gre
        if (xyz[4] == "3")
        {
            Debug.Log("Quit");
            Application.Quit();
        }
    }

    public void StoreName()
    {
        rend = GetComponent<Renderer>();
        rend.material.SetColor("_Color", Color.red);
        theName = inputField.GetComponent<Text>().text;
        textDisplay.GetComponent<Text>().text = "Wpisany nr COM: " + theName;
        stream = new SerialPort(theName, 9600);
        stream.Open(); //Open the Serial Stream.
        stream.ReadTimeout = 100;    //delay
    }

    public string Read()
    {
        receivedstring = stream.ReadLine();
        stream.BaseStream.Flush();
        return receivedstring;
    }
    /*public void Close()
    {
        stream.Close();
    }*/
}