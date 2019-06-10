# *soss-fiware* system handle

## Example of fiware with ros2 connectivity
![](diagrams-design/detailed-diagram.png)

## Class diagram
![]( http://www.plantuml.com/plantuml/png/bLJ1Zjem4BtdAwpj9RGBuhQ78hHIfLKFFLJY0op7cQNkcXdBOrEXgl_U49m1arMbSiBlpNkyUJ5vvcHYoWVVDBiJ9pXyDTXxYEfXlLhXW_guN6xLzznvnpyW_cDcbpksqNMpeP7zGeXW4yL6l1JLX1ZCcEbRyVhcE052R6wyLgjHS8J8Zb2yvanZlM5MTSJVZHe-dvGbV7Nx7A5TA1eKqVLGA7epfpdAmQ4cjF0EDKMOKwOVpibnHVqfqMJlATPoiWh94Ug32PiASFsZiiRxpjYtRVNZCg0YxC-bHNrBj839fTDLlIZgZiYhZE-BxgnhJUXD0dtn5VkVV8uDp6OFYxBIaSPK5jmHsbVtoqIGaS_NBkG1InU9WhCQpG4AK5GwdSBKRxYeMYPcSHLJkQd_vRgyxtJ-c0kIaNCDoQBIH1teUZtLPgFQtUE7bvUdf_LweNOEznxIG6otSalhkjIuPI9zWUkbY-8Mc-I3uWnB-VUfv6yc9KKmlPxkohHPHjcI4mKv-54z7C-6pu3z-8hv2m00)

## Sequence diagram
![](http://www.plantuml.com/plantuml/png/jLR1ZjiW4BtxAtni3_c1LLsjr4iFFLHgxuZWINQq3hWmf4g_linwlMCGYTYelM5uZtdCcvdaCP3s54zJfmqvtpjw0g_2q-kdSHwwEI7GuAmjzJykWU3qLTjnoW--n_s48P5pUDm7ut5VR7znzWm-eBFbjeKKDmT_mq1WqmtTCFHeaL1F-4TJOlV3qB74VlSvK_QGXDi37gC7VWYUDK4kVVF1R0dzq2l5GPJwyB5xsqsGLKU6MA9_sksAy84qnT2DK8z-HRGUKrO8GwDeoNcBf9zkHYCklLDQM8riaoR6vzfkm2H4maaGAI1P5ePvEB8rNhtUC7DfR2uHx3L3VTpYfUBMHOwUNbPvUHI2VaK8b12YcHqHJAL2v33F4lFacXfHJlzjEOOPhNBMtD53Ho35GSNBwmziJiozp-EvGQu_u6_jWKV6EkIA7fgNvLQozClRYaaolr9m5u8YfvRhjsH12PlLP_I5feYxKrvG9nT9ZlEOa0ei8LsgYMweKBcytGT987CaWnME991TE7Xt4X-alKfn1l0CokmR7YN5dn7RHaZ51iL-rbqikxciWficvg7N12x4R70jwkMDPV-xvthlIpOB8tE2f94D6I6GZsRzBRqznL_0xgBKUmTqjEqZ-hyDrNhIP5UcVlS8TkI_H7y1)

## Proyect layout:
- `Dockerfile`: Dockerfile with a system already configured
- `fiware`: main system handler
    - `src` folder: contains the implementation of *soss-fiware* library.
    - `doc` folder: contains the design and user documentation.
    - `sample` folder: contains a *.yaml* file example and utils for running *soss-fiware* easily.
    - `utils` folder: contains scripts and utilities.
- `fiware-test`: system handler for testing the `fiware` system handler.
    - `src` folder: contains the test sources
    - `resources` folder: contains the yaml configurations for the tests.
    - `thirdparty` folder: contains the external code used and distributed with the project.
        - `catch2` folder: contains the *catch2* test framework
