lazy val root = project
  .in(file("."))
  .aggregate(hello)

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
