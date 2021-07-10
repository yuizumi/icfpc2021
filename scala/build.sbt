lazy val root = project
  .in(file("."))
  .aggregate(hello, rot3)
  .settings(
    name := "icfpc2021"
  )

lazy val hello = project
  .in(file("solutions/hello"))
  .settings(
    name := "hello",
    version := "0.1.0",
    scalaVersion := "2.13.5",
    libraryDependencies := Seq(
      "com.softwaremill.sttp.client3" %% "core" % "3.3.9"
    )
  )

lazy val rot3 = project
  .in(file("solutions/rot3"))
  .settings(
    name := "rot3",
    version := "0.1.0",
    scalaVersion := "2.13.5",
    libraryDependencies := Seq(
      "io.circe" %% "circe-core" % "0.14.1",
      "io.circe" %% "circe-generic" % "0.14.1",
      "io.circe" %% "circe-parser" % "0.14.1"
    )
  )

lazy val download = project
  .in(file("tools/download"))
  .settings(
    name := "download",
    version := "0.1.0",
    scalaVersion := "2.13.5",
    libraryDependencies := Seq(
      "com.softwaremill.sttp.client3" %% "core" % "3.3.9"
    )
  )
