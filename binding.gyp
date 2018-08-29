{
    "targets": [
        {
            "target_name": "du",
            "cflags": ["-std=gnu11"],
            "sources": [
                "lib/du_v8.cc",
                "lib/du.c"
            ],
             "include_dirs" : [
                  "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
