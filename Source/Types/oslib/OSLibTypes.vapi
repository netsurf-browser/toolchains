namespace OSLib
{
    public delegate void AsmRoutine ();

    [CCode (cname = "uint", has_type_id = false)]
    public struct bits : uint { }

    [CCode (cname = "char", has_type_id = false)]
    public struct byte : char { }
}
