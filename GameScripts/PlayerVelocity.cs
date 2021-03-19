using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerVelocity   : MonoBehaviour
{
    [SerializeField]
    Vector3 v3Force;

    [SerializeField]
    KeyCode key1;
    [SerializeField]
    KeyCode key2;

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKey(key1))
            GetComponent<Rigidbody>().velocity += v3Force;
        if (Input.GetKey(key2))
            GetComponent<Rigidbody>().velocity -= v3Force;

    }
}
