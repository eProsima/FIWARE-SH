# *soss-fiware* system handle

## Example of fiware with ros2 connectivity
![](diagrams-design/detailed-diagram.png)

## Class diagram
![](http://www.plantuml.com/plantuml/png/bLJ1Rjim3BtxAuJkucnDa5ilGL1WGB733WFo0OOiivbMXVH4AaCmxD_dE38JrTs0vXIzn_V4Hp5vPB5HqY5KrUx40eSl5hi0qTojLokyCn-NowtvbjhW-Jl4_zJiKiik-dPMDBAV2H6SKApKor19gC6CcRvbhw-U1H1YTUErMes28qJsXEerPoeNBBCfM_nTcU7poJZ2PxzF4Ug5eK4HVGUPeXTxcgFSUsm87Rn1JItCATiDP_5SK7zoQvfxQcivMVNY2Pk3xJS5uBjxusm8hNKls-9oBJ0HzkUXnUQMg075o-cgNcHrInHCmhT5hwnBJTzPWURYg_O_-DmsCDiz5EKHVYPWQUJK0zUBdFP8OyCE_17gP__BHhYgTKhwz3ebeTwx1kq1Cf15ets-jh-eQYPcDPs_WIxpk-oNhyS-tuIC9EHKGXfOXrN6lMwmsMnClRl_yFJqyB1UBypEupw03CHsgujSZisCSOMQ2rocpufRR98FYZFypqVGNw8EAOBjcccTfcG9DIKBzNe8eteudWqV0RlnN-Wl)

## Sequence diagram
![](http://www.plantuml.com/plantuml/png/jLTDRzim3BtdLnZOPJiKwAx1KXGOKEommu3jDWo28hE9raJo93fTzkjdEhPfHh3WkMjFYioF9pxoIRsFg0FMnyDA6_IXyBY7eEA--nbzZChu0AkgCRF6LjfXyVKS4OwVj2iFyiENUdkmiOaWpUjjDC5khxQDToS8qNedjZzwvu2IYztFDYAu9W8uj7Wk7kspRb2jDfl2EejM7-mVZTQxOhDP3QYBcpi1Tar97-sk3a07j2UD84yp-a7UZ6PTA4MPb7hxRZNiaaa7JLZauWJuLKD4XUSAOcFQ0s6RSVplRswRluU77tVTcTT515UgBifrgWytD-DSRGxrCtgdIY0IAgHwbv0cJI0pF-ikTGjPeLTjWUd763xR9v14hDyjACOWF9awSfFSHTHO2qT1NOvsNJO9qCQPj7CZ2r9puBxvofh1RH78sCt9J90yLm9bnenJ2E96yydBf6btrEP1KOI-ZtBDcmn7g8Lz_o7TuG1KPeuW-PSk_KAcFD2AilxRF74Dv3H90Bdf8JyE8nsJchzIoyVAEkMTMQ1qEq15cRbByaUTMQ8N79Hq7dt764byVqzT3MjrMOvnyqmEgsjrkyYPLstuAKrZRnOq8Ov7Y57lWCwGTPUrhgjI8wZBjVRosq32dCaCLvDyk9of3tziANaC_ZWYfLinIZ7_O4uZdFaP5ifZm9v0cUqCkU6nIXtpOfSoEjnq4mqXTI9jg7m_oEGz0HTN8R8jVgc9ufZ-QK9NXdhAHf0E_27LrOxavxqz0Cctisv7DB-Ew9Wzc2TrwRZv5ql6SVbzibZYf5zIdMTjKRs_lJqIpXUgOGaHGsrcF7cLQctzcLu7YvuMgV_CLqFjyk-6vJ8tqGiHVVM_UY2_ny0dnc-Il7jm9Vt3zHS0)

## Project layout:
- `Dockerfile`: Dockerfile with a system already configured with ROS2
- `doc` folder: contains the design and user documentation.
- `examples` folder: contains a *.yaml* file example and utils for running *soss-dds* easily.
- `packages`: colcon packages for this middleware
    - `fiware`: main system handler
        - `src` folder: contains the implementation of *soss-fiware* library.
    - `fiware-test`: system handler for testing the `fiware` system handler.
        - `src` folder: contains the test sources
        - `thirdparty` folder: contains the external code used and distributed with the project.
            - `catch2` folder: contains the *catch2* test framework
