/*
 * Examples of things that should work in Java but don't because the required
 * information is lost due to type erasure.
 */
public class ClassErasure<T>
{

    Method findMethod(String name, Class<?>... parameterTypes)
    {
        // should be able to use the Class object from the type parameter
        return T.getMethod(name, parameterTypes); // should be a class<T>?
    }

    void other()
    {
        // another form of using the class object based on the type parameter
        abc(T); // for functions that require a class<T> argument
    }
}
