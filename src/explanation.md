This terminal application aims to provide a simple, educational, and interactive way to explore the concepts of the **Birthday Paradox** and **Birthday Attacks** two closely related phenomena rooted in probability and cryptography.

---

**1. Birthday Paradox Simulation**

This page demonstrates the **Birthday Paradox**, a famous probability puzzle which states that in a group of just 23 people, there's a surprisingly high chance (over 50%) that two people share the same birthday. However, this paradox isn't just limited to birthdays, it applies to any scenario involving random collisions.

In this simulation, you can adjust:

- **Domain Size** - the number of unique values (e.g., 365 days in a year)
- **Sample Count** - the number of samples taken (e.g., number of people)
- **Simulation Runs** - how many times to repeat the experiment to compute average outcomes

The goal is to help you develop an intuition about how quickly collisions occur as sample size increases relative to the domain.

**Use Case:** This principle is widely used in probability theory, data analysis, and even cryptographic risk estimation.

---

**2. Birthday Attack Demo**

This page is a hands-on demo of the **Birthday Attack**, an actual cryptographic attack that exploits the birthday paradox to find hash collisions faster than brute force.

You can experiment with the following options:

- **Custom Hash Functions**: Lightweight toy hash functions with 8-bit, 12-bit, and 16-bit output sizes. You will always see a hash collision if the max attempts are high enough.
- **Real-World Hashes**: Explore cryptographic hashes from the SHA family like **SHA-1**, **SHA-256**, and others (RIPEMD-160). You won't find a hash collision even if you tune the max attempts to a very high number even with birthday attack.

The tool runs a simulation that repeatedly hashes inputs until a collision is found - two different inputs that produce the same hash output.

**Use Case:** Understanding how feasible it is to find collisions in smaller hashes helps people appreciate why modern cryptography uses long hash outputs, and how hash collisions pose a real-world risk to digital signatures and data integrity.

---

**Why This Matters**

Both concepts show how randomness, probability, and hashing interact. Whether you're studying security, building robust applications, or just curious about the math behind it, this simulator offers a direct and visual way to deepen your understanding.
