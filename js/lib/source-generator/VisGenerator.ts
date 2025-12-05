export class VisGenerator {
    public static assembleVisVersionFile(versions: string[]) {
        return `
        ${this.generateVisVersionEnum(versions)}

        ${this.generateVisVersionValues(versions)}

        ${this.generateVisVersionExtensionClass(versions)}

        ${this.generateVisVersionsClass(versions)}
        `;
    }

    private static generateVisVersionEnum(versions: string[]) {
        return `export enum  VisVersion{
            ${versions.map(
                (v) => `${this.toVersionEnumKeyString(v)} = "${v}"\n`
            )}
        }`;
    }

    private static generateVisVersionValues(versions: string[]) {
        return `export const allVisVersions: VisVersion[] = [
            ${versions
                .map((v) => `VisVersion.${this.toVersionEnumKeyString(v)}`)
                .join(",\n")}
        ];`;
    }

    private static generateVisVersionExtensionClass(versions: string[]) {
        return `export class VisVersionExtension {
            public static toVersionString(version: VisVersion, builder?: string[]): string {
              let v: string;
              switch (version) {
                  ${versions
                      .map(
                          (v) => `case VisVersion.${this.toVersionEnumKeyString(
                              v
                          )}:
                  v = "${v}";
                  break;`
                      )
                      .join("")}
                default:
                  throw new Error('Invalid VisVersion enum value: ' + version);
              }

              if (builder) {
                builder.push(v);
              }
              return v;
            }

            public static toString(version: VisVersion, builder?: string[]): string {
              let v: string;
              switch (version) {
                    ${versions
                        .map(
                            (
                                v
                            ) => `case VisVersion.${this.toVersionEnumKeyString(
                                v
                            )}:
                    v = "${v}";
                    break;`
                        )
                        .join("")}
                default:
                  throw new Error('Invalid VisVersion enum value: ' + version);
              }

              if (builder) {
                builder.push(v);
              }
              return v;
            }

            public static isValid(version: VisVersion): boolean {
              switch (version) {
                    ${versions
                        .map(
                            (
                                v
                            ) => `case VisVersion.${this.toVersionEnumKeyString(
                                v
                            )}:
                    return true;\n`
                        )
                        .join("")}
                default:
                  return false;
              }
            }

            public static compare(a: VisVersion, b: VisVersion): number {
                    const aStr = VisVersionExtension.toVersionString(a);
                    const bStr = VisVersionExtension.toVersionString(b);
                    return aStr.localeCompare(bStr, undefined, {
                        numeric: true,
                        sensitivity: "base",
                        ignorePunctuation: true,
                    });
                }

            public static lessThan(a: VisVersion, b: VisVersion): boolean {
                return this.compare(a, b) < 0;
            }

            public static lessThanOrEqual(a: VisVersion, b: VisVersion): boolean {
                return this.compare(a, b) <= 0;
            }

            public static greaterThan(a: VisVersion, b: VisVersion): boolean {
                return this.compare(a, b) > 0;
            }

            public static greaterThanOrEqual(a: VisVersion, b: VisVersion): boolean {
                return this.compare(a, b) >= 0;
            }

            public static equals(a: VisVersion, b: VisVersion): boolean {
                return a === b;
            }

          }`;
    }

    private static generateVisVersionsClass(versions: string[]) {
        return `export class VisVersions {
            public static get all(): VisVersion[] {
              return allVisVersions;
            }

            public static parse(version: string): VisVersion {
              const v = this.tryParse(version);
              if (!v) {
                throw new Error('Couldnt parse version string: ' + version);
              }

              return v;
            }

            public static tryParse(version: string): VisVersion | undefined {
              switch (version) {
                  ${versions
                      .map(
                          (v) => `case "${v}":
                    return VisVersion.${this.toVersionEnumKeyString(v)};\n`
                      )
                      .join("")}
                default:
                  return;
              }
            }


          }`;
    }

    private static toVersionEnumKeyString(v: string) {
        return `v${v.replace("-", "_")}`;
    }
}
