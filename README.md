# Interval booking index

A C++17 reservation index for named resources. Intervals use `[start,end)` semantics: a booking ending at 600 permits another booking to start at 600. Empty or reversed intervals are rejected.

```sh
make test
./booking < fixtures/commands.txt
```

Commands are whitespace-delimited:

```text
book consult room-1 540 600
book review room-1 600 630
gaps room-1 510 660
cancel consult
list room-1
```

Times are signed 64-bit integers in one caller-defined unit. The example uses minutes. Names cannot contain whitespace.

A sorted map per resource checks the adjacent reservations for conflicts in logarithmic time. A separate ID index supports cancellation. Rejected conflicts leave both indexes unchanged. Gap queries clip available intervals to the requested window; listing remains chronological.

Tests cover touching endpoints, containment, spanning conflicts, duplicate IDs, cancellation/rebooking, independent resources, gap clipping and extreme signed endpoints. Native tests and the CLI fixture passed locally with Clang 14.0.6 on Windows. Ubuntu CI uses sanitizers.

The index is in memory and single-threaded. There is no timezone conversion, recurrence engine or database transaction layer.
